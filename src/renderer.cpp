#include "renderer.h"

void Renderer::step_camera() {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) and held_node == (nodeid_t)-1) {
        Vector2 delta = GetMouseDelta() * -1.0f / camera.zoom;
        camera.target += delta;
    }
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

    camera.offset = GetMousePosition();
    camera.target = mouseWorldPos;

    if (held_node == (nodeid_t)-1) {
        float scale = 0.2f*GetMouseWheelMove();
        camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 1/16.f, 64.0f);
    }
}
void Renderer::move_node(Net &net) {
    if (not IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        held_node = -1;
        return;
    }
    if (held_node != (nodeid_t)-1) {
        Node &node = net.nodes.at(held_node);
        //Vector2 position = camera.target;
        //Vector2 direction = position - node.position;
        //push(node, direction);

        Vector2 difference = camera.target - node.position;
        held_angle -= GetMouseWheelMove();
        float angle_diff = held_angle - node.angle;

        node.position += difference * 0.1;
        node.angle += angle_diff * 0.1;
        return;
    }
    Vector2 position = camera.target;
    for (nodeid_t id : net.nodes) {
        Node &node = net.nodes.at(id);
        float distance = Vector2DistanceSqr(node.position, position);
        if (distance < NODE_RADIUS * NODE_RADIUS) {
            held_node = id;
            held_angle = node.angle;
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

            const Port &other = net.get_port(port);

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
    node.rotation *= 0.9;
}
void Renderer::push_nodes(nodeid_t leftid, nodeid_t rightid, Net &net) {
    Node &left = net.nodes.at(leftid);
    Node &right = net.nodes.at(rightid);

    float distance = Vector2Distance(left.position, right.position);
    float force = std::min(PUSH_FORCE, INFLUENCE_RADIUS - distance);
    force += PUSH_FORCE;

    if (force < 0)
        return;

    Vector2 direction = Vector2Normalize(left.position - right.position);

    push(left, direction * force);
    push(right, direction * force * -1);
}
void Renderer::pull_edge(Port fromp, Port top, Net &net) {
    Node &from = net.nodes.at(fromp.nodeid);
    Node &to = net.nodes.at(top.nodeid);

    Vector2 A = port_position(fromp, net);
    Vector2 D = port_position(top, net);

    float force = PUSH_FORCE;
    if (fromp.portid == 0 and top.portid == 0)
        force *= push_mult;

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

    const char *s = net.symbols[node.symbol].text;
    Vector2 text_pos = node.position - net.symbols[node.symbol].text_size;
    DrawText(s, text_pos.x, text_pos.y, NODE_RADIUS, BLACK);
}
void Renderer::draw_edge(Port fromp, Port top, const Net &net) const {
    Vector2 A = port_position(fromp, net);
    Vector2 D = port_position(top, net);

    Vector2 A_dir = port_direction(fromp, net);
    Vector2 D_dir = port_direction(top, net);

    float h = Vector2Length(A - D) / 2;

    Vector2 P0 = A;// + A_dir * NODE_RADIUS / 2;
    Vector2 P3 = D;// + D_dir * NODE_RADIUS / 2;
    Vector2 P1 = P0 + A_dir * h;
    Vector2 P2 = P3 + D_dir * h;

    Color color;
    float width;
    if (fromp.portid == 0 and top.portid == 0) {
        color = RED;
        width = EDGE_WIDTH * 2;
    }
    else {
        color = BLACK;
        width = EDGE_WIDTH;
    }
    //DrawSplineSegmentLinear(A, P0, width, color);
    //DrawCircleV(P0, width/2, color);
    DrawSplineSegmentBezierCubic(P0, P1, P2, P3, width, color);
    //DrawCircleV(P3, width/2, color);
    //DrawSplineSegmentLinear(D, P3, width, color);
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
