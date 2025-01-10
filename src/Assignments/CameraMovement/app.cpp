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

    set_camera(new Camera);
    set_controler(new CameraControler(camera()));

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
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // apex
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // pyramid walls
         0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
         1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    };

    glm::vec3 camera_position = { 0,0,3 };
    glm::vec3 zero = { 0,0,0 };
    glm::vec3 up_vector = { 0,1,0 };
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    float aspect = (float)w / h;
    float fov = 1.0 * glm::radians(90.0f);
    float near = 0.1f;
    float far = 100.0f;
    std::tie(w, h) = frame_buffer_size();
    camera()->look_at(camera_position, zero, up_vector);
    camera()->perspective(fov, aspect, near, far);

    glGenBuffers(1, &transformation_pvm_u_buffer);
    GLuint transformationsBlockIndex = glGetUniformBlockIndex(program, "Transformations");

    glBindBuffer(GL_UNIFORM_BUFFER, transformation_pvm_u_buffer);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, transformation_pvm_u_buffer);
    glUniformBlockBinding(program, transformationsBlockIndex, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint modifier_u_buffer_handle;
    GLuint modifierBlockIndex = glGetUniformBlockIndex(program, "Modifier");

    float strength = 0.5f;
    float color[3] = { 1,0,0 };
    glGenBuffers(1, &modifier_u_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, modifier_u_buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, modifier_u_buffer_handle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(strength), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), sizeof(color), color);
    glUniformBlockBinding(program, modifierBlockIndex, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint vertex_buffer_handle;
    glGenBuffers(1, &vertex_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint indices_buffer_handle;
    glGenBuffers(1, &indices_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLshort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_handle);

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
    auto PVM = camera()->projection() * camera()->view();
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
    camera()->set_aspect((float)w / h);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}