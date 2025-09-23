#include "animator.h"

void Animator::step_net(Net &net) {
    for (auto &edge : net.edges) {
        pull_edge(edge, net);
    }
    for (auto &[id1, left] : net.nodes) {
        for (auto &[id2, right] : net.nodes) {
            push_away_nodes(left, right);
        }
    }
    for (auto &[id, node] : net.nodes) {
        step_node(node);
    }
}

void Animator::step_node(Node &node) {
    node.position += node.velocity;
    node.angle += node.rotation;

    node.velocity *= 0.99;
    node.rotation *= 0.99;
}

void Animator::push_at_point(Node &node, Vector2 force, Vector2 point) {
    node.velocity += force / Node::mass;

    Vector2 r = point - node.position;
    float torque = r.x * force.y - r.y * force.x;

    node.rotation += torque / Node::inertia;
}

void Animator::pull_edge(const Edge &edge, Net &net) {
    Node &from = net.nodes.at(edge.from.nodeid);
    Node &to = net.nodes.at(edge.to.nodeid);

    auto [A, _1] = get_port_offsets(from, edge.from.portid, net);
    auto [D, _2] = get_port_offsets(to, edge.to.portid, net);

    //float distance = Vector2Distance(A, D);
    float force;

    if (edge.is_cut()) {
        force = force_cut;
    }
    else {
        force = force_noncut;
    }
    Vector2 dir = Vector2Normalize(from.position - to.position);

    push_at_point(from, dir * force * -1, A);
    push_at_point(to, dir * force, D);
}

void Animator::push_away_nodes(Node &left, Node &right) {
    float distance = Vector2DistanceSqr(left.position, right.position);
    float limit = Node::radius * 8.f;

    if (distance > limit * limit)
        return;

    distance = sqrt(distance);

    float force = (distance / limit - 1) * (distance / limit - 1) * 64;
    Vector2 direction = Vector2Normalize(left.position - right.position);

    push_at_point(left, direction * force, left.position);
    push_at_point(right, direction * force * -1, right.position);
}
