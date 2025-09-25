#pragma once

#include "lambda_net.h"

class Renderer {
    float force_noncut = 32;
    float force_cut = 32;

    nodeid_t held_node = (nodeid_t)-1;
public:
    Camera2D camera = {};

    Renderer(Vector2 screensize) {
        camera.offset = (Vector2){ screensize.x/2.0f, screensize.y/2.0f };
        camera.zoom = 1.0f;
    }
    void step_camera();
    void move_node(Net &net);
    void step_net(Net &net);
    void draw_net(const Net &net) const;
private:
    void step_node(nodeid_t id, Net &net);
    void push_nodes(nodeid_t leftid, nodeid_t rightid, Net &net);
    void pull_edge(Port from, Port to, Net &net);
    void draw_node(nodeid_t id, const Net &net) const;
    void draw_edge(Port from, Port to, const Net &net) const;

    void push(Node &node, Vector2 force);
    void push_at_point(Node &node, Vector2 force, Vector2 point);
};
