#include "lambda_net.h"
#include "graphics.h"

void control_camera(Camera2D &camera) {
    int camera_speed = 10;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta() * -1.0f/camera.zoom;
        camera.target += delta;
    }
    Vector2 delta = {0,0};

    if (IsKeyDown(KEY_D)) delta.x += 1;
    if (IsKeyDown(KEY_S)) delta.y += 1;
    if (IsKeyDown(KEY_A)) delta.x -= 1;
    if (IsKeyDown(KEY_W)) delta.y -= 1;

    camera.target += Vector2Normalize(delta) * camera_speed / camera.zoom;

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

    camera.offset = GetMousePosition();
    camera.target = mouseWorldPos;
    float scale = 0.2f*GetMouseWheelMove();
    camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 1/16.f, 64.0f);
}

void draw_edge(const Port &left, const Port &right, const Net &net) {
    if (left.type == None or right.type == None)
        return;

    Vector2 A = net[left].position + port_rotated(left.type, net[left].angle);
    Vector2 D = net[right].position + port_rotated(right.type, net[right].angle);

    Vector2 diff = A - D;
    Vector2 normed = Vector2Normalize(diff);
    float size = Vector2Length(diff) / 4 + EDGE_SIZE;

    float a = Vector2DotProduct(normed, Vector2Rotate({1,0}, net[left].angle)) + 2;
    float d = Vector2DotProduct(normed, Vector2Rotate({-1,0}, net[right].angle)) + 2;

    Vector2 B = A + port_rotated_offset(left.type, net[left].angle) * a * size;
    Vector2 C = D + port_rotated_offset(right.type, net[right].angle) * d * size;

    if (left.type == Main and right.type == Main)
        DrawSplineSegmentBezierCubic(A, B, C, D, EDGE_SIZE*2, RED);
    else
        DrawSplineSegmentBezierCubic(A, B, C, D, EDGE_SIZE, BLACK);
}

void draw_net(const Net &net) {
    // Draw Edges

    for (auto [i, node] : net.nodes) {
        draw_edge({Main, i},  node.main,  net);
        draw_edge({Left, i},  node.left,  net);
        draw_edge({Right, i}, node.right, net);
    }

    // Draw Nodes
    for (auto [i, node] : net.nodes) {
        DrawPoly(node.position, 3, NODE_SIZE+EDGE_SIZE*2, node.angle*TODEG, BLACK);
        DrawPoly(node.position, 3, NODE_SIZE, node.angle*TODEG, colors[node.symbol]);
    }
}
