//
// Created by pbialas on 25.09.2020.
//


#include "app.h"

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glad/gl.h"

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    auto program = xe::utils::create_program(
        {
                {GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
                {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}
        });

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    std::vector<GLshort> indices = {
        0,1,2,
        1,2,3,
        1,3,4
     };
    std::vector<GLfloat> vertices = {
        // roof
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        //walls
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f
    };

    GLuint modifier_u_buffer_handle;
    GLuint modifierBlockIndex = glGetUniformBlockIndex(program, "Modifier");
    glGenBuffers(1, &modifier_u_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, modifier_u_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, modifier_u_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint tranformations_pvm_u_buffer_handle;
    GLuint transformationsBlockIndex = glGetUniformBlockIndex(program, "Transformations");
    glGenBuffers(1, &tranformations_pvm_u_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, tranformations_pvm_u_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, tranformations_pvm_u_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint vertex_buffer_object;
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint index_buffer_handle;
    glGenBuffers(1, &index_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLshort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),reinterpret_cast<GLvoid*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindBuffer(GL_UNIFORM_BUFFER, modifier_u_buffer_handle);
    float strength = 0.5f;
    float color[3] = { 1,0,0 };
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(strength), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), sizeof(color), color);
    glUniformBlockBinding(program, modifierBlockIndex, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    glBindBuffer(GL_UNIFORM_BUFFER, tranformations_pvm_u_buffer_handle);

    glm::mat4 PVM(1.0f);
    glm::mat4 M(1.0f);
    glm::vec3 camera_pos = { 0,2,2 };
    glm::vec3 zero = { 0,0,0 };
    glm::vec3 up_vec = { 0,0,1 };
    glm::mat4 V;
    glm::mat4 P;
    V = glm::lookAt(camera_pos, zero, up_vec);
    P = glm::perspective(1.0f * glm::radians(45.0), 1.0 * frame_buffer_size().first / frame_buffer_size().second, 0.1, 100.0);

    PVM = P * V * M;

    glm::mat4 T = glm::translate(PVM, glm::vec3(1, 1, 0));

    PVM = T;


    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PVM), &PVM);
    glUniformBlockBinding(program, transformationsBlockIndex, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_handle);
    glBindVertexArray(0);
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    auto [w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}