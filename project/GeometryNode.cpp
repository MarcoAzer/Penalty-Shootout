#include <iostream>
#include <glm/gtx/io.hpp>
#include "cs488-framework/GlErrorCheck.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "GeometryNode.hpp"
#include <stdio.h>
#include "lodepng/lodepng.h"
#include <map>
#include <glm/gtx/transform.hpp>

BatchInfoMap* GeometryNode::batchInfoMap = NULL;
ShaderProgram* GeometryNode::shader = NULL;
glm::mat4* GeometryNode::view = NULL;
GLuint GeometryNode::texture = -1;
bool* GeometryNode::drawCube = NULL;

struct Image{
    unsigned char* data;
    unsigned w;
    unsigned h;
};

std::map< unsigned int, Image > TEXTURES;


//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId),
      firstTime(true)
{
	m_nodeType = NodeType::GeometryNode;

    cube[0] = glm::vec4( 1, -1, -1, 1 ); // Right Back
    cube[1] = glm::vec4( 1, -1, 1, 1 ); // Right Front
    cube[2] = glm::vec4( -1, -1, 1, 1 ); // Left Front
    cube[3] = glm::vec4( -1, -1, -1, 1 ); // Left Back
    cube[4] = glm::vec4( 1, 1, -1, 1 ); // Top right back
    cube[5] = glm::vec4( 1, 1, 1, 1 ); // Top right front
    cube[6] = glm::vec4( -1, 1, 1, 1 ); // Top left front
    cube[7] = glm::vec4( -1, 1, -1, 1 ); // Top left back
}

void
GeometryNode::draw() const
{
    shader->enable();
    {
        //-- Set ModelView matrix:
        GLint location = shader->getUniformLocation("ModelView");
        glm::mat4 modelView = *view * transformations.top(); glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
        CHECK_GL_ERRORS;

        //-- Set NormMatrix:
        location = shader->getUniformLocation("NormalMatrix");
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelView)));
        glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
        CHECK_GL_ERRORS;

        if( textureFile != "" ){
            location = shader->getUniformLocation("hasTexture");
            glUniform1i(location, 1);
            CHECK_GL_ERRORS;
			
            if( TEXTURES.find(m_nodeId) == TEXTURES.end() ){
                unsigned char* image;
                unsigned h = 0, w = 0;

                int status = lodepng_decode_file( &image, &w, &h, textureFile.c_str(), LCT_RGB, 8 );
                if( status ){
                    std::cout << "Error reading file: " << textureFile << std::endl;
                }
                else{
                    Image textureImage = { image, w, h };
                    TEXTURES[m_nodeId] = textureImage;
                }
            }

            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TEXTURES[m_nodeId].w, TEXTURES[m_nodeId].h, 0, GL_RGB, GL_UNSIGNED_BYTE, TEXTURES[m_nodeId].data );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            location = shader->getUniformLocation( "myTexture" );
            glUniform1i( location, 1 );

        }
        else{
            location = shader->getUniformLocation("hasTexture");
            glUniform1i(location, 0);
            CHECK_GL_ERRORS;
            //-- Set Material values:
            location = shader->getUniformLocation("material.kd");
            glm::vec3 kd = material.kd;
            glUniform3fv(location, 1, value_ptr(kd));
            CHECK_GL_ERRORS;
            location = shader->getUniformLocation("material.ks");
            glm::vec3 ks = material.ks;
            glUniform3fv(location, 1, value_ptr(ks));
            CHECK_GL_ERRORS;
            location = shader->getUniformLocation("material.shininess");
            glUniform1f(location, material.shininess);
        }
        CHECK_GL_ERRORS;
    }

    shader->disable();

    // Get the BatchInfo corresponding to the GeometryNode's unique MeshId
    BatchInfo batchInfo = (*batchInfoMap)[meshId];

    // Render the mesh
    shader->enable();
    glDrawArrays( GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices );
    shader->disable();

    if( *drawCube ){
        batchInfo = (*batchInfoMap)["uvcube"];
        shader->enable();
        glDrawArrays( GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices );
        shader->disable();
    }
}

void GeometryNode::traverse()
{
    glm::mat4 M = transformations.top() * trans;

    //if( moving || firstTime ){
        translateCube( M );
        //firstTime = false;
    //}

    transformations.push( M );

    draw();

    for( SceneNode* child : children )
        child->traverse();

    transformations.pop();
}

void GeometryNode::translateCube( glm::mat4 M )
{
    //std::cout << m_name << " Cube Start" << std::endl;
    for( int i = 0; i < 8; i++ ){
        newCube[i] = glm::vec3( M * cube[i] );
        //std::cout << newCube[i] << std::endl;
    }
    //std::cout << "Cube End" << std::endl;
}

void GeometryNode::reset(){
    newCube[0] = glm::vec3(0, 0, 0);
    newCube[1] = glm::vec3(0, 0, 0);
    newCube[2] = glm::vec3(0, 0, 0);
    newCube[3] = glm::vec3(0, 0, 0);
    newCube[4] = glm::vec3(0, 0, 0);
    newCube[5] = glm::vec3(0, 0, 0);
    newCube[6] = glm::vec3(0, 0, 0);
    newCube[7] = glm::vec3(0, 0, 0);
}

void GeometryNode::setTexture( char* fileName ){
    if( TEXTURES.find(m_nodeId) != TEXTURES.end() ){
        delete[] TEXTURES[m_nodeId].data;
        TEXTURES.erase(m_nodeId);
    }

    textureFile = fileName;
}

void GeometryNode::setColour( glm::vec3 colour ){
    material.kd = colour;
}

GLuint
GeometryNode::loadTexture( std::string fileName ) const
{
    FILE* file = fopen( fileName.c_str(), "r" );
    if( !file ){
        std::cout << "Couldn't open " << file << std::endl;
        return -1;
    } 

    unsigned char header[54];
    if( fread( header, 1, 54, file ) != 54 ){
        std::cout << "Not a correct bmp file" << std::endl;
        return -1;
    }

    if( header[0] != 'B' || header[1] != 'M' ){
        std::cout << "Not a correct bmp file" << std::endl;
        return -1;
    }
	int dataPos    = *(int*)&(header[0x0A]);
	int imageSize  = *(int*)&(header[0x22]);
	int width      = *(int*)&(header[0x12]);
	int height     = *(int*)&(header[0x16]);

	if( imageSize == 0 )
		imageSize = height * width * 3;
	
	if( dataPos == 0 )
        dataPos = 54;

    unsigned char data[imageSize];
    fread( data, 1, imageSize, file );

    fclose( file );
    
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return textureID;
}
