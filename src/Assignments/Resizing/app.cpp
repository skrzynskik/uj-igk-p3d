//
// Created by pbialas on 25.09.2020.
//


#include "app.h"
#include <vector>
#include "glad/gl.h"
#include "glm/gtc/matrix_transform.hpp"
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
        0, 1, 2,
        1, 3, 2,
        4, 6, 5,
        7, 9, 8,
        10, 11, 12,
        13, 14, 15
    };
    std::vector<GLfloat> vertices = {
        // pyramid base
        1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f,
        1.0f, -1.0f, 0.0f, 0.5f, 0.5f, 0.5f,
       -1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f,
       -1.0f, -1.0f, 0.0f, 0.5f, 0.5f, 0.5f,
       // 1st wall
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
       -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       //2nd wall
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
       //3rd wall
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
       -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
       //4th wall
       -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
       -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
   };

    glm::mat4 PVM(1.0f);
    glm::mat4 M(1.0);
    glm::vec3 camera_position = { 0,0,3 };
    glm::vec3 zero = { 0,0,0 };
    glm::vec3 up_vector = { 0,1,0 };
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    aspect_ = (float)w / h;
    fov_ = glm::pi<float>() / 4.0;
    near_ = 0.1f;
    far_ = 100.0f;
    P_ = glm::perspective(fov_, aspect_, near_, far_);
    V_ = glm::lookAt(camera_position, zero, up_vector);
    //P = glm::perspective(1.0 * glm::radians(90.0f), 1.0 * frame_buffer_size().first / frame_buffer_size().second, 0.1, 20.0);

    PVM = P_ * V_ * M;

    glm::mat4 T = glm::translate(PVM, glm::vec3(0, 0, 0));

    PVM = T;
    glGenBuffers(1, &transformation_pvm_u_buffer);
    GLuint transformationsBlockIndex = glGetUniformBlockIndex(program, "Transformations");
    glBindBuffer(GL_UNIFORM_BUFFER, transformation_pvm_u_buffer);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, transformation_pvm_u_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PVM), &PVM);
    glUniformBlockBinding(program, transformationsBlockIndex, 1);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint m_buffer_handle;
    GLuint modifierBlockIndex = glGetUniformBlockIndex(program, "Modifier");
    float strength = 0.5f;
    float color[3] = { 1,0,0 };
    glGenBuffers(1, &m_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_buffer_handle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(strength), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), sizeof(color), color);
    glUniformBlockBinding(program, modifierBlockIndex, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint i_buffer_handle;
    glGenBuffers(1, &i_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLshort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    glViewport(0, 0, w, h);

    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void SimpleShapeApplication::frame() {
    auto PVM = P_ * V_;
    glBindBuffer(GL_UNIFORM_BUFFER, transformation_pvm_u_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    aspect_ = (float)w / h;
    P_ = glm::perspective(fov_, aspect_, near_, far_);
}