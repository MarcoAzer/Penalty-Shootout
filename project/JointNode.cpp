#include <iostream>
#include "cs488-framework/MathUtils.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/io.hpp>
#include "JointNode.hpp"

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

void JointNode::traverse()
{
    glm::mat4 M = transformations.top() * trans;

    transformations.push( M );

    for( SceneNode* child : children )
        child->traverse();

    transformations.pop();
}

float get_angle( float angle, float inc, float min, float max )
{
    float n_angle = angle + inc;
    if( n_angle > max )
        return max - angle;
    else if( n_angle < min )
        return min - angle;
    else
        return inc;
}

float JointNode::rotate(char axis, float angle) 
{
    glm::vec3 rot_axis;
    float rotate_by;

	switch (axis) {
		case 'x':
			rot_axis = glm::vec3(1,0,0);
            rotate_by = get_angle( rotated.x, angle, m_joint_x.min, m_joint_x.max );
            rotated.x += rotate_by;
			break;
		case 'y':
			rot_axis = glm::vec3(0,1,0);
            rotate_by = get_angle( rotated.y, angle, m_joint_y.min, m_joint_y.max );
            rotated.y += rotate_by;
	        break;
		case 'z':
			rot_axis = glm::vec3(0,0,1);
            rotate_by = angle;
            rotated.z += angle;
	        break;
		default:
			break;
	}

    glm::mat4 rot_matrix = glm::rotate(degreesToRadians(rotate_by), rot_axis);
    glm::mat4 prev_trans = trans;

    glm::vec3 v_trans( 0, translated.y * scaled.y , 0 );
    trans = glm::inverse( prev_trans ) * trans;
    trans = glm::translate( v_trans ) * trans;
    trans = rot_matrix * trans;
    v_trans *= -1;
    trans = glm::translate( v_trans ) * trans;
    trans = prev_trans * trans;

    return rotate_by;
}

