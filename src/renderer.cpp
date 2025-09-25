#include "renderer.h"

void Renderer::step_camera() {
    int camera_speed = 10;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) and held_node == (nodeid_t)-1) {
        Vector2 delta = GetMouseDelta() * -1.0f / camera.zoom;
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
void Renderer::move_node(Net &net) {
    if (not IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        held_node = -1;
        return;
    }
    if (held_node != (nodeid_t)-1) {
        Node &node = net.nodes.at(held_node);
        Vector2 position = camera.target;
        Vector2 direction = position - node.position;
        push(node, direction);
        return;
    }
    Vector2 position = camera.target;
    for (nodeid_t id : net.nodes) {
        Node &node = net.nodes.at(id);
        float distance = Vector2DistanceSqr(node.position, position);
        if (distance < NODE_RADIUS * NODE_RADIUS) {
            held_node = id;
            return;
        }
    }
}
void Renderer::step_net(Net &net) {
    for (nodeid_t id1 : net.nodes) {
        for (nodeid_t id2 : net.nodes) {
            push_nodes(id1, id2, net);
        }
        const Node &node = net.nodes[id1];
        for (const Port &port : node.ports) {
            if (port.nodeid > id1)
                continue;

            const Port &other = net.nodes[port.nodeid].ports[port.portid];

            if (port.nodeid == id1 and other.portid > port.portid)
                continue;

            pull_edge(port, other, net);
        }
    }
    for (nodeid_t id : net.nodes) {
        step_node(id, net);
    }
}
void Renderer::draw_net(const Net &net) const {
    for (nodeid_t id : net.nodes) {
        const Node &node = net.nodes[id];
        for (const Port &port : node.ports) {
            if (port.nodeid > id)
                continue;

            const Port &other = net.nodes[port.nodeid].ports[port.portid];

            if (port.nodeid == id and other.portid > port.portid)
                continue;

            draw_edge(port, other, net);
        }
    }
    for (nodeid_t id : net.nodes) {
        draw_node(id, net);
    }
}
void Renderer::step_node(nodeid_t id, Net &net) {
    Node &node = net.nodes.at(id);

    node.position += node.velocity;
    node.angle += node.rotation;

    node.velocity *= 0.99;
    node.rotation *= 0.99;
}
void Renderer::push_nodes(nodeid_t leftid, nodeid_t rightid, Net &net) {
    Node &left = net.nodes.at(leftid);
    Node &right = net.nodes.at(rightid);

    float distance = Vector2Distance(left.position, right.position);
    float limit = NODE_RADIUS * 8.f;

    if (distance > limit)
        return;

    float force = (distance / limit - 1) * (distance / limit - 1) * 64;
    Vector2 direction = Vector2Normalize(left.position - right.position);

    push(left, direction * force);
    push(right, direction * force * -1);
}
void Renderer::pull_edge(Port fromp, Port top, Net &net) {
    Node &from = net.nodes.at(fromp.nodeid);
    Node &to = net.nodes.at(top.nodeid);

    Vector2 A = port_position(fromp, net);
    Vector2 D = port_position(top, net);

    float force;

    if (fromp.portid == 0 and top.portid == 0)
        force = force_cut;
    else
        force = force_noncut;
    Vector2 dir = Vector2Normalize(from.position - to.position);

    push_at_point(from, dir * force * -1, A);
    push_at_point(to, dir * force, D);
}
void Renderer::draw_node(nodeid_t id, const Net &net) const {
    const Node &node = net.nodes.at(id);
    Color color = net.symbols.at(node.symbol).color;
    Color border = id == held_node ? RED : BLACK;
    portid_t ports = net.symbols.at(node.symbol).ports;
    if (ports <= 1) {
        float radius = NODE_RADIUS * CRC_TRG_RATIO;
        DrawCircleV(node.position, radius + EDGE_WIDTH, border);
        DrawCircleV(node.position, radius, color);
    }
    else {
        DrawPoly(node.position, 3, NODE_RADIUS + EDGE_WIDTH * 2, node.angle*TODEG, border);
        DrawPoly(node.position, 3, NODE_RADIUS, node.angle*TODEG, color);
    }

    Font font = GetFontDefault();

    std::string s = std::to_string(id);
    Vector2 text_size = MeasureTextEx(font, s.c_str(), 20, 0) / 2;
    DrawText(s.c_str(), node.position.x - text_size.x, node.position.y - text_size.y, 20, BLACK);
}
void Renderer::draw_edge(Port fromp, Port top, const Net &net) const {
    Vector2 A = port_position(fromp, net);
    Vector2 D = port_position(top, net);

    Vector2 A_dir = port_direction(fromp, net);
    Vector2 D_dir = port_direction(top, net);

    float h = Vector2Length(A - D) / 2;

    Vector2 B = A + A_dir * h;
    Vector2 C = D + D_dir * h;

    if (fromp.portid == 0 and top.portid == 0)
        DrawSplineSegmentBezierCubic(A, B, C, D, EDGE_WIDTH * 2, RED);
    else
        DrawSplineSegmentBezierCubic(A, B, C, D, EDGE_WIDTH, BLACK);
}

void Renderer::push(Node &node, Vector2 force) {
    node.velocity += force / NODE_MASS;
}
void Renderer::push_at_point(Node &node, Vector2 force, Vector2 point) {
    node.velocity += force / NODE_MASS;

    Vector2 r = point - node.position;
    float torque = r.x * force.y - r.y * force.x;

    node.rotation += torque / NODE_INERTIA;
}
