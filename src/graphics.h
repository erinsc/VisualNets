#pragma once

#include "lambda_net.h"

void control_camera(Camera2D &camera);
void draw_net(const Net &net);
void physics_step_net(Net &net, bool interact);
void update_varports(Net &net);

constexpr float NODE_SIZE = 32;
constexpr float EDGE_SIZE = 8;
constexpr float TODEG = 180 / PI;
constexpr float STRENGTH = 0.5f;

constexpr Color colors[6] = {{255,127,127,255}, {255,191,127,255}, {255,255,127,255},
                             {127,255,127,255}, {127,255,255,255}, {127,127,127,255}};

constexpr Vector2 port_offsets[3] = {
    { NODE_SIZE, 0},
    {-NODE_SIZE/2, -NODE_SIZE/2},
    {-NODE_SIZE/2,  NODE_SIZE/2}
};

inline Vector2 port_rotated(Type type, float angle) {
    return Vector2Rotate(port_offsets[type], angle);
}
inline Vector2 port_rotated_offset(Type type, float angle) {
    if (type == Main)
        return Vector2Rotate({1,0}, angle);
    return Vector2Rotate({1,0}, angle + PI);
}

