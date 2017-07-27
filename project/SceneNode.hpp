#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>
#include <stack>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    virtual float rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

    virtual void traverse();

	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

	bool isSelected;
    
    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

    static glm::mat4* M;

    glm::vec3 translated;
    glm::vec3 scaled;
    glm::vec3 rotated;
    SceneNode* parent;

    virtual void setColour( glm::vec3 colour );

protected:
    static std::stack< glm::mat4 > transformations;

private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
