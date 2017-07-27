#pragma once

#include <glm/glm.hpp>

class Physics{
  public:
    glm::vec3 pos_i;
    glm::vec3 prevPos;
    float vx_i, vy_i, vz_i;
    glm::vec3 wind;
    Physics(): pos_i(0.0, 0.0, 0.0){}
    void reset();
    void setParabolic( float vi, float angle_x, float angle_y, glm::vec3 wind );
    glm::vec3 getTrans( float t );
};
