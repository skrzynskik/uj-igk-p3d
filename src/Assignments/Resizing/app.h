//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include "Application/application.h"
#include "Application/utils.h"
#include "glad/gl.h"
#include "glm/gtc/matrix_transform.hpp"


class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug) {}

    void init() override;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;


private:
    GLuint vao_;
    float fov_;
    float aspect_;
    float near_;
    float far_;

    glm::mat4 P_;
    glm::mat4 V_;
    GLuint transformation_pvm_u_buffer;

};