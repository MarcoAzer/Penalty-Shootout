#include <iostream>
#include "Physics.hpp"
#include <math.h>
#include "cs488-framework/MathUtils.hpp"

void
Physics::reset(){
    pos_i.x = pos_i.y = pos_i.z = 0.0;
    prevPos.x = prevPos.y = prevPos.z = 0.0;
}

void
Physics::setParabolic( float vi, float angle_x, float angle_y, glm::vec3 wind ){
    vx_i = vi * sin( degreesToRadians( -1 * angle_y ) );
    vy_i = vi * sin( degreesToRadians( angle_x ) );
    vz_i = -1 * ( vi * cos( degreesToRadians( angle_x ) ) );
    this->wind = wind;
}

glm::vec3
Physics::getTrans( float t ){
    glm::vec3 newPos;
    newPos.x = pos_i.x + ( t * vx_i ) + ( t * wind.x );
    newPos.y = pos_i.y + ( vy_i * t ) - ( 0.5 * 9.8 * pow( t, 2 ) ) + ( t * wind.y );
    newPos.z = pos_i.z + t * vz_i + ( t * wind.z );
    
    glm::vec3 trans = newPos - prevPos;
    prevPos = newPos;
    return trans;
}
