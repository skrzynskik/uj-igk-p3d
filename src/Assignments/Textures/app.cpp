#include "app.h"
#include <vector>
#include "glad/gl.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Application/utils.h"
#include "Engine/Mesh.h"
#include "Engine/Material.h"

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
    xe::ColorMaterial::init();
    set_camera(new Camera);
    set_controler(new CameraControler(camera()));
    auto pyramid = new xe::Mesh;

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
         1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        // 1st wall
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,
        //2nd wall
         1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,
        //3rd wall
         1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,
        //4th wall
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,
    };


    pyramid->allocate_vertex_buffer(vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data());
    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), 0);

    pyramid->allocate_index_buffer(indices.size() * sizeof(GLshort), GL_STATIC_DRAW);
    pyramid->load_indices(0, indices.size() * sizeof(GLshort), indices.data());

    auto base_material = new xe::ColorMaterial(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    auto side_material_1 = new xe::ColorMaterial(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    auto side_material_2 = new xe::ColorMaterial(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    auto side_material_3 = new xe::ColorMaterial(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    auto side_material_4 = new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    pyramid->add_submesh(0, 3, base_material);
    pyramid->add_submesh(3, 6, base_material);
    pyramid->add_submesh(6, 9, side_material_1);
    pyramid->add_submesh(9, 12, side_material_2);
    pyramid->add_submesh(12, 15, side_material_3);
    pyramid->add_submesh(15, 18, side_material_4);

    add_submesh(pyramid);

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

    for (auto m : meshes_)
        m->draw();
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