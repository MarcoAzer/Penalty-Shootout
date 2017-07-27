#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Physics.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <map>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class A5 : public CS488Window {
public:
	A5();
	virtual ~A5();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile();
	void createShaderProgram();
	void enableVertexShaderInputSlots();
    void enableUVCoordinates();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(SceneNode &node);

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;
    glm::vec3 lightPos;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
    GLuint m_vbo_vertexUV;
    GLuint m_vbo_tangents;
    GLuint m_vbo_bitangents;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
    GLint m_uvAttribLocation;
    GLint m_tangentsAttribLocation;
    GLint m_bitangentsAttribLocation;
	ShaderProgram m_shader;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

    std::map< std::string, GeometryNode* > geo_nodes;
    void get_geo_nodes( SceneNode& );

    // Misc
    bool zbuffer, backface_culling, frontface_culling;

    // God Mode
    bool godMode;
    glm::mat4 v_trans;
    bool translateView( int key );

    // User Interface
    SceneNode* jArrow;
    bool left, right, up, down;
    float angle_x, angle_y;
    float vi;
    glm::vec3 wind;

    // Skybox and field
	std::shared_ptr<SceneNode> m_rootSkybox;
    GeometryNode* m_skybox;
    GeometryNode* m_field;
    GeometryNode* gWall1;
    GLuint m_skyboxID, m_fieldID, m_wallID;
    void initSkybox();
    void renderSkybox();
    void initPlane( GLuint id, const char* fileName, const char* fraMap, int textureId );
    void renderPlane( GeometryNode* obj, int textureId );

    void resetScene();

    // Ball Physics
    Physics ballPhysics;
    SceneNode* jBall;
    GeometryNode* gBall;
    bool moveBall;
    float time;
    bool newTrajectory;

    GeometryNode* gTopPost, *gRightPost, *gLeftPost;

    // Collision detection
    bool drawCube;

    SceneNode* jPuppet;

    // Players
    const char* currentPlayer;
    int bScore, eScore;
    GeometryNode *gBTenth, *gBUnit, *gETenth, *gEUnit;
    void updateScore();
    void switchPlayers();
    void setGoalieOutfit( glm::vec3 topColour, glm::vec3 bottomColour );
    bool alreadyIntersect;

    void animateGoalie();
    void translateGoalie();
    void jumpGoalie();
    bool animate, translate, jump;
};
