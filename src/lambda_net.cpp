#include "lambda_net.h"

nodeid_t Net::insert_node(const Node &node) {
    nodes[id_counter] = node;
    return id_counter++;
}
template <typename ...Ts>
nodeid_t Net::emplace_node(Ts &&...args) {
    nodes[id_counter] = {std::forward<Ts>(args)...};
    return id_counter++;
}

std::pair<Vector2, Vector2> get_port_offsets(const Node &node, portid_t port, const Net &net) {
    portid_t ports = net.symbols[node.symbol].ports;

    Vector2 offset;
    float angle;
    if (ports == 1) {
        offset = {CRC_TRG_RATIO, 0};
        angle = node.angle;
    }
    else if (port == 0){
        offset = {1, 0};
        angle = node.angle;
    }
    else {
        offset = {1.f/2, 2.f * port / ports - 1};
        angle = node.angle + PI;
    }
    Vector2 point = Vector2Rotate(offset, angle) * Node::radius + node.position;
    Vector2 dir = Vector2Rotate({1,0}, angle);

    return {point, dir};
}
