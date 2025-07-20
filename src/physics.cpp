#include "lambda_net.h"
#include "graphics.h"

void push_at_point(Node &node, Vector2 force, Vector2 point) {
    node.velocity += force;

    Vector2 r = point - node.position;
    float tau = r.x * force.y - r.y * force.x;

    node.rotation += tau / (NODE_SIZE * NODE_SIZE);
}
void physics_edge(const Port &left, const Port &right, Net &net, bool interact) {
    Vector2 left_pos = net[left].position + port_rotated(left.type, net[left].angle);
    Vector2 right_pos = net[right].position + port_rotated(right.type, net[right].angle);

    //float limit = NODE_SIZE * 8;
    float distance = Vector2Distance(left_pos, right_pos);

    float force = STRENGTH / 16;

    if (left.type == Main and right.type == Main and interact) {
        if (distance < NODE_SIZE) {
            net.interact(left, right);
            return;
        }
        force *= 8;
    }


    Vector2 direction = Vector2Normalize(left_pos - right_pos);

    push_at_point(net[left], direction * (-force), left_pos);
    push_at_point(net[right], direction * (force), right_pos);
}

void physics_nodes(size_t left, size_t right, Net &net) {
    float distance = Vector2DistanceSqr(net[left].position, net[right].position);
    float limit = NODE_SIZE * 8.f;

    if (distance > limit * limit)
        return;

    distance = sqrt(distance);

    float force = STRENGTH * (distance / limit - 1) * (distance / limit - 1);
    Vector2 direction = Vector2Normalize(net[left].position - net[right].position);

    net[left].velocity += direction * force;
    net[right].velocity -= direction * force;
}

void physics_step_net(Net &net, bool interact) {
    // Node to Node repelling
    for (size_t l = 0; l < net.nodes.capacity()-1; ++l) {
        if (net[l].is_index())
            continue;
        for (size_t r = l+1; r < net.nodes.capacity(); ++r) {
            if (net[r].is_index())
                continue;
            physics_nodes(l, r, net);
        }
    }
    // Edge attraction
    for (size_t n = 0; n < net.nodes.capacity()-1; ++n) {
        if (net[n].is_index())
            continue;
        physics_edge({Main, n}, net[n].main, net, interact);
        physics_edge({Left, n}, net[n].left, net, interact);
        physics_edge({Right, n}, net[n].right, net, interact);
    }
    // Update position and angle
    for (auto [n, node] : net.nodes) {
        if (n == 0)
            continue;

        node.velocity *= 0.99;
        node.rotation *= 0.9;

        node.position += node.velocity;
        node.angle += node.rotation;
    }
}
