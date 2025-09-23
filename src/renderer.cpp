#include "renderer.h"

void Renderer::control_camera() {
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

void Renderer::draw_net(const Net &net) const {
    for (auto &edge : net.edges) {
        draw_edge(edge, net);
    }

    for (auto &[id, node] : net.nodes) {
        draw_node(id, node, net);
    }
}

void Renderer::draw_node(nodeid_t id, const Node &node, const Net &net) const {

    Color color = net.symbols.at(node.symbol).color;
    portid_t ports = net.symbols.at(node.symbol).ports;
    if (ports <= 1) {
        DrawCircleV(node.position, Node::radius*CRC_TRG_RATIO+Edge::width, BLACK);
        DrawCircleV(node.position, Node::radius*CRC_TRG_RATIO, color);
    }
    else {
        DrawPoly(node.position, 3, Node::radius+Edge::width*2, node.angle*TODEG, BLACK);
        DrawPoly(node.position, 3, Node::radius, node.angle*TODEG, color);
    }

    std::string s = std::to_string(id);
    DrawText(s.c_str(), node.position.x, node.position.y, 20, BLACK);
}

void Renderer::draw_edge(const Edge &edge, const Net &net) const {
    const Node &from = net.nodes.at(edge.from.nodeid);
    const Node &to = net.nodes.at(edge.to.nodeid);

    auto [A, A_dir] = get_port_offsets(from, edge.from.portid, net);
    auto [D, D_dir] = get_port_offsets(to, edge.to.portid, net);

    Vector2 diff = A - D;

    float a = Vector2DotProduct(diff, Vector2Rotate({1,0}, from.angle)) / 4 + Vector2Length(diff);
    float d = -Vector2DotProduct(diff, Vector2Rotate({1,0}, to.angle)) / 4 + Vector2Length(diff);

    Vector2 B = A + A_dir * a;
    Vector2 C = D + D_dir * d;

    if (edge.is_cut())
        DrawSplineSegmentBezierCubic(A, B, C, D, Edge::width*2, RED);
    else
        DrawSplineSegmentBezierCubic(A, B, C, D, Edge::width, BLACK);
}
