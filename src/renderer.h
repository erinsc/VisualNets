#pragma once

#include "lambda_net.h"

class Renderer {
public:
    Camera2D camera;

    Renderer(Vector2 screensize) {
        camera = {};
        camera.zoom = 1.f;
        camera.offset = (Vector2){ screensize.x/2.0f, screensize.y/2.0f };
        camera.target = (Vector2){ screensize.x, screensize.y };
    }

    void control_camera();
    void draw_net(const Net &net) const;
private:
    void draw_node(nodeid_t id, const Node &node, const Net &net) const;
    void draw_edge(const Edge &edge, const Net &net) const;
};
