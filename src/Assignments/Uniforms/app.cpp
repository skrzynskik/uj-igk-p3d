//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <tuple>
#include <numeric>

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    //indices

    std::vector<GLubyte> indices = {
        0,1,2,
        1,2,3,
        1,3,4
    };

    // A vector containing the x,y,z vertex coordinates for the triangle.
    std::vector<GLfloat> vertices = {
        // roof
        0.0f, 0.5f, 0.0f,   
        1,0,0,

        -0.5f, 0.0f, 0.0f,  
        1,0,0,

        0.5f, 0.0f, 0.0f,
        1,0,0,

        // bottom side of the base
        0.5f, -0.5f, 0.0f,
        0,1,0,

        // Top right side of the home base
        -0.5f, -0.5f, 0.0f,
        0,1,0
    };

    //generating the buffer and loading the index data into it.
    GLuint i_buffer_handle;
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Generating buffer for modifer_uniform in vs_shader
    GLuint modifier_uniform_buffer_handle;
    glGenBuffers(1, &modifier_uniform_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, modifier_uniform_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, modifier_uniform_buffer_handle);

    float strength = 0.75;
    float color[3] = {1.0, 1.0, 2.0};

    glBufferSubData(GL_UNIFORM_BUFFER, 0, 4*sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4*sizeof(float), 4*sizeof(float), color);

    //Generating buffer for transformation_uniform in vertex shader
    GLuint transformations_uniform_buffer_handle;
    glGenBuffers(1, &transformations_uniform_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, transformations_uniform_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, transformations_uniform_buffer_handle);

    float theta = 1.0 * glm::pi<float>()/6.0f;
    auto cs = std::cos(theta);
    auto ss = std::sin(theta);  
    glm::mat2 rot{cs,ss,-ss,cs};
    glm::vec2 trans{0.0,  -0.25};
    glm::vec2 scale{0.5, 0.5};

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(scale), &scale);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), sizeof(trans), &trans);
    glBufferSubData(GL_UNIFORM_BUFFER, 8 * sizeof(float), sizeof(rot), &rot);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    // This indicates that the data for attribute 0 should be read from a vertex buffer.
    glEnableVertexAttribArray(0);
    // and this specifies how the data is layout in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

        // Enable and specify the layout of the color attribute (attribute 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_BYTE, 0);
    glBindVertexArray(0);
}
