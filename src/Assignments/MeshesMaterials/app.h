//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"
#include "camera.h"
#include "camera_controler.h"
#include "glad/gl.h"
#include "Engine/Mesh.h"

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug) {}

    void init() override;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override {
         Application::scroll_callback(xoffset, yoffset);
         camera()->zoom(yoffset / 30.0f);
    }


    void set_camera(Camera *camera) { camera_ = camera; }
    void set_controler(CameraControler *controler) { controler_ = controler; }

    virtual void cursor_position_callback(double x, double y) override;

    virtual void mouse_button_callback(int button, int action, int mods) override;

    Camera *camera() { return camera_; }
    
    void add_submesh(xe::Mesh *mesh) {
        meshes_.push_back(mesh);
    }

    ~SimpleShapeApplication() {
        if (camera_) {
            delete camera_;
        }
        if (!meshes_.empty()) {
            for (auto p : meshes_)
            {
                delete p;
            }
            meshes_.clear();
        }
    }

private:
    Camera* camera_;
    CameraControler *controler_;

    GLuint vao_;
    GLuint transformation_pvm_u_buffer;

    std::vector<xe::Mesh*> meshes_; 
};