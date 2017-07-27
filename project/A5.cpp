#include "A5.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include <SDL2/SDL.h>

#include "lodepng/lodepng.h"

using namespace glm;

static bool show_gui = true;
static bool cubesIntersect( glm::vec3* cube1, glm::vec3* cube2 );
static void* playSoundThread( void* musicFile );
static void setScore( GeometryNode* tenth, GeometryNode* unit, int score );

//----------------------------------------------------------------------------------------
// Constructor
A5::A5():
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
      jArrow(0),
      jBall(0),
      zbuffer(true),
      backface_culling(false),
      frontface_culling(false),
      godMode(false),
      m_skybox(0),
      m_field(0),
      angle_x(0),
      angle_y(0),
      vi(10.0),
      wind(vec3(0)),
      moveBall(false),
      time(0.0),
      drawCube( false ),
      jPuppet(0),
      currentPlayer("Brazil"),
      bScore(0),
      eScore(0),
      alreadyIntersect(false),
      lightPos(vec3(-2.0f, 5.0f, 0.5f)),
      animate(false),
      translate(false),
      jump(false)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile();

    get_geo_nodes( *m_rootNode );

    setGoalieOutfit( vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0) );

    initSkybox();
    initPlane( m_fieldID, "Assets/field.png", "fieldNormalMap", 2 );
    initPlane( m_wallID, "Assets/wall.png", "wallNormalMap", 4 );

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			//getAssetFilePath("cube.obj"),
			//getAssetFilePath("sphere.obj"),
			getAssetFilePath("plane.obj"),
			getAssetFilePath("uvsphere.obj"),
			getAssetFilePath("uvcube.obj"),
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

    GeometryNode::batchInfoMap = &m_batchInfoMap;
    GeometryNode::shader = &m_shader;
    GeometryNode::view = &m_view;
    GeometryNode::drawCube = &drawCube;

    GLuint texture;
    glGenTextures(1, &texture);
    GeometryNode::texture = texture;
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture(GL_TEXTURE_2D, texture);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A5::processLuaSceneFile() {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua("soccer.lua"));
	if (!m_rootNode) {
		std::cerr << "Could not open soccer.lua" << std::endl;
	}
    m_rootSkybox = std::shared_ptr<SceneNode>(import_lua("skybox.lua"));
	if (!m_rootSkybox) {
		std::cerr << "Could not open skybox.lua" << std::endl;
	}
    for(SceneNode* child : m_rootSkybox->children ){
        if( child->m_name == "jSkybox" ){
            m_skybox = dynamic_cast<GeometryNode*>( child->children.front() );
        }
        else if( child->m_name == "jField" ){

            m_field = dynamic_cast<GeometryNode*>( child->children.front() );
        }
        else{
            gWall1 = dynamic_cast<GeometryNode*>( child->children.front() );
        }
    }
    if( !m_skybox && !m_field )
        std::cerr << "Could not cast skybox or field" << std::endl;
}

//----------------------------------------------------------------------------------------
void A5::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A5::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

        // Enable the vertes shader attribute location for "uv" when rendering.
        m_uvAttribLocation = m_shader.getAttribLocation("uv");
        glEnableVertexAttribArray( m_uvAttribLocation );

		m_tangentsAttribLocation = m_shader.getAttribLocation("tangents");
		glEnableVertexAttribArray(m_tangentsAttribLocation);

		m_bitangentsAttribLocation = m_shader.getAttribLocation("bitangents");
		glEnableVertexAttribArray(m_bitangentsAttribLocation);

		CHECK_GL_ERRORS;
	}


	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A5::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
    
    // Generate VBO to store all vertex uv coordinates
    {
        glGenBuffers( 1, &m_vbo_vertexUV );

        glBindBuffer( GL_ARRAY_BUFFER, m_vbo_vertexUV );

        glBufferData( GL_ARRAY_BUFFER, meshConsolidator.getNumVertexUVBytes(), meshConsolidator.getVertexUVDataPtr(), GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        CHECK_GL_ERRORS;
    }

    {
        vector<vec3> vertices = meshConsolidator.getVertexPositionData();
        vector<vec2> uvs = meshConsolidator.getVertexUVData();

        vector<vec3> tangents, bitangents;
        for( int i = 0; i < vertices.size(); i += 3 ){
            vec3& v0 = vertices[i];
            vec3& v1 = vertices[i+1];
            vec3& v2 = vertices[i+2];

            vec2& uv0 = uvs[i];
            vec2& uv1 = uvs[i+1];
            vec2& uv2 = uvs[i+2];

            vec3 deltaV1 = v1 - v0;
            vec3 deltaV2 = v2 - v0;

            vec2 deltaUV1 = uv1 - uv0;
            vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        	vec3 tangent = normalize( r * (deltaV1 * deltaUV2.y   - deltaV2 * deltaUV1.y) );
        	vec3 bitangent = normalize( r * (deltaV2 * deltaUV1.x   - deltaV1 * deltaUV2.x) );
        	//vec3 tangent = r * (deltaV1 * deltaUV2.y   - deltaV2 * deltaUV1.y);
        	//vec3 bitangent = r * (deltaV2 * deltaUV1.x   - deltaV1 * deltaUV2.x); 
			
			tangents.push_back( tangent );
			tangents.push_back( tangent );
			tangents.push_back( tangent );
			bitangents.push_back( bitangent );
			bitangents.push_back( bitangent );
			bitangents.push_back( bitangent );
        }

        glGenBuffers( 1, &m_vbo_tangents );
        glBindBuffer( GL_ARRAY_BUFFER, m_vbo_tangents );
        glBufferData( GL_ARRAY_BUFFER, tangents.size() * sizeof(vec3), &(tangents[0]), GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        CHECK_GL_ERRORS;
		
		
        glGenBuffers( 1, &m_vbo_bitangents );
        glBindBuffer( GL_ARRAY_BUFFER, m_vbo_bitangents );
        glBufferData( GL_ARRAY_BUFFER, bitangents.size() * sizeof(vec3), &(bitangents[0]), GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        CHECK_GL_ERRORS;
    }
}

//----------------------------------------------------------------------------------------
void A5::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Tell GL how to map data from vertext buffer "m_vbo_vertexUV" into the 
    // "uv" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUV);
    glVertexAttribPointer(m_uvAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_tangents);
	glVertexAttribPointer(m_tangentsAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bitangents);
	glVertexAttribPointer(m_bitangentsAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;


	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A5::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 15000.0f);
}


//----------------------------------------------------------------------------------------
void A5::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 8.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A5::initLightSources() {
	// World-space position
	m_light.position = lightPos;
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A5::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
        m_view = glm::lookAt(vec3(0.0f, 0.0f, 8.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f) );
        m_view = glm::inverse( v_trans ) * m_view;

		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();

    if( animate )
        animateGoalie();
    if( translate )
        translateGoalie();

    m_light.position = lightPos;
    if( moveBall ){
        if( gBall->newCube[4].y < -0.7 ){
            moveBall = false;
        }
        // Local time to trajectory
        if( newTrajectory ){
            newTrajectory = false;
            time = 0.0;
        }
        time += 0.005;
        vec3 trans = ballPhysics.getTrans(time);
        gBall->moving = true;
        jBall->translate(trans);
        // Prevent from re-executing when nothing much has changes, because called 60 times per second
        if( alreadyIntersect ){
            return;
        }
        bool intersect;
        for( map< string, GeometryNode* >::iterator it = geo_nodes.begin(); it != geo_nodes.end(); it++ ){
            intersect = cubesIntersect( gBall->newCube, it->second->newCube );
            if( intersect ){
                alreadyIntersect = true;
                ballPhysics.setParabolic( 0.8 * vi, 180, angle_y, wind );
                ballPhysics.pos_i = ballPhysics.prevPos;
                newTrajectory = true;
                if( it->second->m_name == "gTopPost" || 
                    it->second->m_name == "gLeftPost" ||
                    it->second->m_name == "gRightPost" ){
                    
                    const char* musicFile = "Assets/ball_hit_post.wav";
                    pthread_t thread;
                    pthread_create( &thread, NULL, playSoundThread, (void*)musicFile );
                    
                }
                else{ // Hit goalie
                    const char* musicFile = "Assets/goalie_catch_ball.wav";
                    pthread_t thread;
                    pthread_create( &thread, NULL, playSoundThread, (void*)musicFile );
                }
                break;
            }
        }
        // Detecting goal
        if( !intersect ){
            // Passed goal line
            if( gBall->newCube[1].z < gTopPost->newCube[0].z ){
                alreadyIntersect = true;
                if( gBall->newCube[1].x < gRightPost->newCube[3].x &&
                    gBall->newCube[3].x > gLeftPost->newCube[1].x &&
                    gBall->newCube[4].y < gTopPost->newCube[0].y ){
                    const char* musicFile = "Assets/crowd_cheer.wav";
                    pthread_t thread;
                    pthread_create( &thread, NULL, playSoundThread, (void*)musicFile );

                    updateScore();
                }
                else{
                    const char* musicFile = "Assets/crowd_booing.wav";
                    pthread_t thread;
                    pthread_create( &thread, NULL, playSoundThread, (void*)musicFile );
                }
            }
        }
    }
    else{
        gBall->moving = false;
    }
}

struct Edge{
    glm::vec3& A;
    glm::vec3& B;
};

struct Triangle{
    glm::vec3& A;
    glm::vec3& B;
    glm::vec3& C;
};

static bool cubesIntersect( glm::vec3* cube1, glm::vec3* cube2 ){
    Edge edges[12] = {
        { cube1[0], cube1[1] },
        { cube1[1], cube1[2] },
        { cube1[2], cube1[3] },
        { cube1[3], cube1[1] },
        { cube1[4], cube1[5] },
        { cube1[5], cube1[6] },
        { cube1[6], cube1[7] },
        { cube1[7], cube1[4] },
        { cube1[1], cube1[4] },
        { cube1[2], cube1[5] },
        { cube1[3], cube1[6] },
        { cube1[4], cube1[7] }
    };

    Triangle triangles[12] = {
        { cube2[1], cube2[3], cube2[0] },
        { cube2[7], cube2[5], cube2[4] },
        { cube2[4], cube2[1], cube2[0] },
        { cube2[5], cube2[2], cube2[1] },
        { cube2[2], cube2[7], cube2[3] },
        { cube2[0], cube2[7], cube2[4] },
        { cube2[1], cube2[2], cube2[3] },
        { cube2[7], cube2[6], cube2[5] },
        { cube2[4], cube2[5], cube2[1] },
        { cube2[5], cube2[6], cube2[2] },
        { cube2[2], cube2[6], cube2[7] },
        { cube2[0], cube2[3], cube2[7] }
    };

    bool intersect;
    glm::vec3 position;

    for( int i = 0; i < 12; i++ ){
        for( int j = 0; j < 12; j++ ){
           intersect = glm::intersectRayTriangle( edges[i].A, normalize(edges[i].B - edges[i].A), triangles[j].A, triangles[j].B, triangles[j].C, position ); 
           if( intersect && position.z < 0.4 ){
               //cout << "Edge: " << edges[i].A << ", " << edges[i].B << endl;
               //cout << "Triangle: " << triangles[j].A << ", " << triangles[j].B << ", " << triangles[j].C << endl;
               //cout << "Position: " << position << endl;
               return true;
           }
        }
    }
    return false;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...
        if( ImGui::BeginMainMenuBar() ){
            if( ImGui::BeginMenu( "Game" ) ){
                if( ImGui::MenuItem( "New Game" ) ){
                    resetScene();
                    bScore = 0;
                    eScore = 0;
                    setScore( gBTenth, gBUnit, bScore );
                    setScore( gETenth, gEUnit, eScore );
                    animate = false;
                    translate = false;
                }
                else if( ImGui::MenuItem( "Quit" ) ){
                    glfwSetWindowShouldClose(m_window, GL_TRUE);
                }
                ImGui::EndMenu();
            }
            else if( ImGui::BeginMenu( "GoalKeeper" ) ){
                ImGui::MenuItem( "Animate", "", &animate );
                ImGui::MenuItem( "Translate", "", &translate );
                ImGui::EndMenu();
            }
            else if( ImGui::BeginMenu( "Options" ) ){
                ImGui::MenuItem( "Z-Buffer", "Z", &zbuffer );
                ImGui::MenuItem( "Backface culling", "B", &backface_culling );
                ImGui::MenuItem( "Frontface culling", "F", &frontface_culling );
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Text( "Current Player: %s", currentPlayer );
        ImGui::Text( "Shot parameters" );
        ImGui::Text( "Angle X: %.0f degrees", angle_x );
        ImGui::Text( "Angle Y: %.0f degrees", angle_y );
        ImGui::SliderFloat( "Speed", &vi, 10, 100, "%.0f" );
        ImGui::Text("Wind parameters");
        ImGui::SliderFloat( "Wind X", &wind.x, -10, 10, "%.0f" );
        ImGui::SliderFloat( "Wind Y", &wind.y, -10, 10, "%.0f" );
        ImGui::SliderFloat( "Wind Z", &wind.z, -10, 10, "%.0f" );
        ImGui::Text( "Light parameters" );
        ImGui::SliderFloat( "Light X", &lightPos.x, -50, 50, "%.0f" );
        ImGui::SliderFloat( "Light Y", &lightPos.y, -50, 50, "%.0f" );
        ImGui::SliderFloat( "Light Z", &lightPos.z, -50, 50, "%.0f" );

 
		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;
        
		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A5::draw() {
    // Z buffer
    if( zbuffer )
	    glEnable( GL_DEPTH_TEST );

    // Face culling
    if( backface_culling || frontface_culling )
    {
        glEnable( GL_CULL_FACE );

        if( backface_culling && frontface_culling )
            glCullFace( GL_FRONT_AND_BACK );
        else if( backface_culling )
            glCullFace( GL_BACK );
        else if( frontface_culling )
            glCullFace( GL_FRONT );
    }

    renderSceneGraph(*m_rootNode);
    renderSkybox();
    renderPlane( m_field, 2 );
    renderPlane( gWall1, 4 );

    if( backface_culling || frontface_culling)
        glDisable( GL_CULL_FACE );

    if( zbuffer )
	    glDisable( GL_DEPTH_TEST );
}

//----------------------------------------------------------------------------------------
void A5::renderSceneGraph(SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

    root.traverse();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A5::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A5::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A5::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A5::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A5::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A5::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A5::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);


    if( key == GLFW_KEY_G && action == GLFW_PRESS ){
        godMode = !godMode;
        eventHandled = true;
    }
    else if( action == 2 ){
        if( godMode ){
            eventHandled = translateView( key );
        }
        else{
            switch( key ){
                case GLFW_KEY_LEFT:
                    jArrow->rotate( 'y', 1 );
                    angle_y += 1;
                    break;
                case GLFW_KEY_RIGHT:
                    jArrow->rotate( 'y', -1 );
                    angle_y -= 1;
                    break;
                case GLFW_KEY_UP:
                    jArrow->rotate( 'x', -1 );
                    angle_x -= 1;
                    break;
                case GLFW_KEY_DOWN:
                    jArrow->rotate( 'x', 1 );
                    angle_x += 1;
                    break;
            }
        }
    }
    else if( action == GLFW_PRESS ) {
        if( godMode ){
            eventHandled = translateView( key );
        }
        else{
            switch( key ){
                case GLFW_KEY_M:
                    show_gui = !show_gui;
                    break;
                case GLFW_KEY_Q:
                    glfwSetWindowShouldClose(m_window, GL_TRUE);
                    break;
                case GLFW_KEY_B:
                    backface_culling = !backface_culling;
                    break;
                case GLFW_KEY_F:
                    frontface_culling = !frontface_culling;
                    break;
                case GLFW_KEY_Z:
                    zbuffer = !zbuffer;
                    break;
                case GLFW_KEY_L:
	                m_light.position = vec3(-20.0f, 5.0f, 0.5f);
                    break;
                case GLFW_KEY_R:
	                m_light.position = vec3(20.0f, 5.0f, 0.5f);
                    break;
                case GLFW_KEY_SPACE:
                    {
                        newTrajectory = true;
                        ballPhysics.setParabolic( vi, angle_x, angle_y, wind );
                        moveBall = !moveBall;
                        const char* musicFile = "Assets/kicking_soccer_ball.wav";
                        pthread_t thread;
                        pthread_create( &thread, NULL, playSoundThread, (void*)musicFile );
                        if( jump )
                            jumpGoalie();
                    }
                    break;
                case GLFW_KEY_D:
                    drawCube = !drawCube;
                    break;
                case GLFW_KEY_S:
                    alreadyIntersect = false;
                    switchPlayers();
                    resetScene();
                    break;
                default:
                    return eventHandled;
            }
        }
	}

    eventHandled = true;
	return eventHandled;
}

void A5::get_geo_nodes( SceneNode& node )
{
    // Get the arrow rotate it around
    if( node.m_name == "Arrow" ){
        jArrow = &node;
        return;
    }
    else if( node.m_name == "jBall" ){
        jBall = &node;
        gBall = static_cast<GeometryNode*>(jBall->children.front());
        return;
    }
    else if( node.m_name == "jPuppet" ){
        jPuppet = &node;
    }
    else if( node.m_name == "gTopPost" ){
        gTopPost = static_cast<GeometryNode*>(&node);
    }
    else if( node.m_name == "gRightPost" ){
        gRightPost = static_cast<GeometryNode*>(&node);
    }
    else if( node.m_name == "gLeftPost" ){
        gLeftPost = static_cast<GeometryNode*>(&node);
    }
    else if( node.m_name == "gBTenth" ){
        gBTenth = static_cast<GeometryNode*>(&node);
    }
    else if( node.m_name == "gBUnit" ){
        gBUnit = static_cast<GeometryNode*>(&node);
    }
    else if( node.m_name == "gETenth" ){
        gETenth = static_cast<GeometryNode*>(&node);
    }
    else if( node.m_name == "gEUnit" ){
        gEUnit = static_cast<GeometryNode*>(&node);
    }

    if( node.m_nodeType == NodeType::GeometryNode )
        geo_nodes[node.m_name] = static_cast<GeometryNode*>(&node);
    else
        for( SceneNode* child : node.children )
            get_geo_nodes( *child );
}

bool A5::translateView( int key ){
    bool eventHandled = true;
    vec3 trans_by;
    float c = 1.0;
    switch( key ){
        case GLFW_KEY_LEFT:
            trans_by.x -= c;
            break;
        case GLFW_KEY_RIGHT:
            trans_by.x += c;
            break;
        case GLFW_KEY_UP:
            trans_by.z -= c;
            break;
        case GLFW_KEY_DOWN:
            trans_by.z += c;
            break;
        case GLFW_KEY_EQUAL:
            trans_by.y += c;
            break;
        case GLFW_KEY_MINUS:
            trans_by.y -= c;
            break;
        default:
            eventHandled = false;
            break;
    }

    v_trans = v_trans * glm::translate( mat4(), trans_by );
    vec3 y_axis( 0, 1, 0 );
    vec3 x_axis( 1, 0, 0 );
    c = 5;
    switch( key ){
        case GLFW_KEY_A:
            v_trans = v_trans * glm::rotate( mat4(), glm::radians( c ), y_axis );
            break;
        case GLFW_KEY_D:
            v_trans = v_trans * glm::rotate( mat4(), glm::radians( -c ), y_axis );
            break;
        case GLFW_KEY_W:
            v_trans = v_trans * glm::rotate( mat4(), glm::radians( -c ), x_axis );
            break;
        case GLFW_KEY_S:
            v_trans = v_trans * glm::rotate( mat4(), glm::radians( c ), x_axis );
            break;
        default:
            eventHandled = false;
            break;
    }
    return eventHandled;
}

void A5::initSkybox(){
    const char* fileNames[6] = {
        "Assets/xpos.png",
        "Assets/xneg.png",
        "Assets/ypos.png",
        "Assets/yneg.png",
        "Assets/zpos.png",
        "Assets/zneg.png"
    };
    unsigned w, h;
    unsigned char* data;
    int status;

    glGenTextures(1, &m_skyboxID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxID);
    for( GLuint i = 0; i < 6; i++ ){
        status = lodepng_decode_file( &data, &w, &h, fileNames[i], LCT_RGB, 8 );
        if( !status ){
            glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
            delete []data;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_shader.enable();
    glUniform1i(m_shader.getUniformLocation("skybox"), 3);
    m_shader.disable();
}

void A5::renderSkybox(){
    glBindVertexArray(m_vao_meshData);

    updateShaderUniforms( m_shader, *m_skybox, m_view * m_skybox->parent->trans );

    m_shader.enable();
    glUniform1i(m_shader.getUniformLocation("hasTexture"), 3);
    CHECK_GL_ERRORS;
    m_shader.disable();

    BatchInfo batchInfo = m_batchInfoMap[m_skybox->meshId];

    m_shader.enable();
    glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
    m_shader.disable();

    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}

void A5::initPlane( GLuint id, const char* fileName, const char* fragMap, int textureId ){
    unsigned w, h;
    unsigned char* data;

    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0 + textureId);
    glBindTexture(GL_TEXTURE_2D, id);
    int status = lodepng_decode_file(&data, &w, &h, fileName, LCT_RGB, 8);
    if( !status ){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        delete []data;
    }
    else{
        cout << "Can't open file: " << fileName << endl;
    }

    m_shader.enable();
    glUniform1i(m_shader.getUniformLocation(fragMap), textureId);
    m_shader.disable();
}

void A5::renderPlane( GeometryNode* obj, int hasTexture ){
    glBindVertexArray(m_vao_meshData);

    updateShaderUniforms( m_shader, *obj, m_view * obj->parent->trans );

    m_shader.enable();
    glUniform1i(m_shader.getUniformLocation("hasTexture"), hasTexture);
    CHECK_GL_ERRORS;
    m_shader.disable();

    BatchInfo batchInfo = m_batchInfoMap[obj->meshId];

    m_shader.enable();
    glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
    m_shader.disable();

    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}

void A5::resetScene(){
    vec3 transBack = -1.0f * jBall->translated;
    jBall->translate(transBack);
    jBall->translate( vec3( 0.0, -0.7, 5.0 ) );
    gBall->reset();
    ballPhysics.reset();
    angle_x = angle_y = 0;
    vi = 10.0;
    jArrow->rotate('y', -jArrow->rotated.y);
    jArrow->rotate('x', -jArrow->rotated.x);
}

struct AudioData{
    Uint8* pos;
    Uint32 length;
};

void audioCallback( void* userdata, Uint8* stream, int streamLength ){
    AudioData* audio = (AudioData*)userdata;

    if( audio->length == 0 ){
        return;
    }

    Uint32 length = (Uint32)streamLength;
    length = (length > audio->length ? audio->length : length);

    SDL_memcpy( stream, audio->pos, length );
    audio->pos += length;
    audio->length -= length;
}

static void* playSoundThread( void* mFile ){
    const char* musicFile = (char*)mFile;

    if( SDL_Init(SDL_INIT_AUDIO) < 0 ){
        cout << "Couldn't init SDL Audio" << endl;
        return NULL;
    }

    SDL_AudioSpec wavSpec;
    Uint8* wavStart;
    Uint32 wavLength;

    if( SDL_LoadWAV( musicFile, &wavSpec, &wavStart, &wavLength ) == NULL ){
        cout << "Couldn't load music file" << endl;
        return NULL;
    }

    AudioData audio;
    audio.pos = wavStart;
    audio.length = wavLength;

    wavSpec.callback = audioCallback;
    wavSpec.userdata = &audio;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice( NULL, 0, &wavSpec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE );

    if( device == 0 ){
        cout << "Couldn't use audio device" << endl;
        return NULL;
    }

    SDL_PauseAudioDevice(device, 0);

    while(audio.length > 0){
        SDL_Delay(100);
    }

    SDL_CloseAudioDevice( device );
    SDL_FreeWAV(wavStart);
    SDL_Quit();
    return NULL;
}

static void setScore( GeometryNode* tenth, GeometryNode* unit, int score ){
    int unitScore = score % 10; 
    int tenthScore = score / 10;

    char fileName[13];
    snprintf(fileName, 13, "Assets/%d.png", unitScore);
    unit->setTexture(fileName);

    if( tenthScore ){
        snprintf(fileName, 13, "Assets/%d.png", tenthScore);
        tenth->setTexture(fileName);
    }
}

void A5::updateScore(){
    if( !strncmp(currentPlayer, "Brazil", 6) ){
        bScore += 1;
        setScore( gBTenth, gBUnit, bScore );
    }
    else if( !strncmp(currentPlayer, "Egypt", 5) ){
        eScore += 1;
        setScore( gETenth, gEUnit, eScore );
    }
}

void A5::switchPlayers(){
    if( !strncmp(currentPlayer, "Brazil", 6) ){
        setGoalieOutfit( vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0) );
        currentPlayer = "Egypt";
    }
    else if( !strncmp(currentPlayer, "Egypt", 5) ){
        setGoalieOutfit( vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0) );
        currentPlayer = "Brazil";
    }
}

void A5::setGoalieOutfit( glm::vec3 topColour, glm::vec3 bottomColour ){
    geo_nodes["Torso"]->setColour( topColour );
    geo_nodes["Shoulders"]->setColour( topColour );
    geo_nodes["LeftUpperArm"]->setColour( topColour );
    geo_nodes["LeftForearm"]->setColour( topColour );
    geo_nodes["RightUpperArm"]->setColour( topColour );
    geo_nodes["RightForearm"]->setColour( topColour );

    geo_nodes["Hips"]->setColour( bottomColour );
    geo_nodes["LeftThigh"]->setColour( bottomColour );
    geo_nodes["RightThigh"]->setColour( bottomColour );
}

void A5::animateGoalie(){
    static const float change = 1.5;
    static float angle = 0;
    static bool goingUp = true;

    if(angle == 180){
        goingUp = false;
    }
    else if( angle == 0 ){
        goingUp = true;
    }

    if( goingUp ){
        geo_nodes["LeftUpperArm"]->parent->rotate( 'z', change );
        geo_nodes["RightUpperArm"]->parent->rotate( 'z', -change );
        geo_nodes["LeftThigh"]->parent->rotate( 'z', change/2 );
        geo_nodes["RightThigh"]->parent->rotate( 'z', -change/2);
        angle += change;
    }
    else{
        geo_nodes["LeftUpperArm"]->parent->rotate( 'z', -change );
        geo_nodes["RightUpperArm"]->parent->rotate( 'z', change );
        geo_nodes["LeftThigh"]->parent->rotate( 'z', -change/2);
        geo_nodes["RightThigh"]->parent->rotate( 'z', change/2);
        angle -= change;
    }
}

void A5::translateGoalie(){
    static float position = 10.0;
    static bool goingLeft = false;

    float transChange = rand() % 10 + 1;
    transChange /= 10;

    if( position >= 20.00 ){
        goingLeft = true;
    }
    else if( position <= 0 ){
        goingLeft = false;
    }

    if( goingLeft ){
        geo_nodes["Torso"]->parent->translate(vec3(-transChange, 0, 0));
        position -= transChange;
    }
    else{
        geo_nodes["Torso"]->parent->translate(vec3(transChange, 0, 0));
        position += transChange;
    }

}

void A5::jumpGoalie(){
    static SceneNode* goalie = geo_nodes["Torso"]->parent;
    static SceneNode* leftShoulder = geo_nodes["LeftUpperArm"]->parent;
    static SceneNode* leftElbow = geo_nodes["LeftForearm"]->parent;
    static SceneNode* rightShoulder = geo_nodes["RightUpperArm"]->parent;
    static SceneNode* rightElbow = geo_nodes["RightForearm"]->parent;

    static SceneNode* leftHip = geo_nodes["LeftThigh"]->parent;
    static SceneNode* leftKnee = geo_nodes["LeftCalf"]->parent;
    static SceneNode* rightHip = geo_nodes["RightThigh"]->parent;
    static SceneNode* rightKnee = geo_nodes["RightCalf"]->parent;

    // Frame one
    leftElbow->rotate( 'x', -90 );
    rightElbow->rotate( 'x', -90 );
    leftHip->rotate( 'x', -90 );
    leftKnee->rotate( 'x', 90 );
    rightHip->rotate( 'x', -90 );
    rightKnee->rotate( 'x', 90 );
    goalie->translate( vec3(0, -1, 0 ));

    usleep(1000000);

    // Frame Two
    leftElbow->rotate( 'x', 20 );
    rightElbow->rotate( 'x', 20 );
    leftHip->rotate( 'x', 20 );
    leftKnee->rotate( 'x', -20 );
    rightHip->rotate( 'x', 20 );
    rightKnee->rotate( 'x', -20 );
    goalie->rotate( 'z', -20 );
    goalie->translate( vec3(1, 1, 0 ));
}
