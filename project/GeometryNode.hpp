#pragma once

#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "glm/glm.hpp"
#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

    glm::vec4 cube[8];
    glm::vec3 newCube[8];
    void translateCube( glm::mat4 M );
    bool firstTime, moving;

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;
    std::string textureFile;

    static BatchInfoMap* batchInfoMap;
    static ShaderProgram* shader;
    static glm::mat4* view;
    static GLuint texture;
    static bool* drawCube;

    void draw() const;
    virtual void traverse();
    void reset();
    void setTexture( char* fileName );
    virtual void setColour( glm::vec3 colour );

    GLuint loadTexture( std::string fileName ) const;
};
