#include "lambda_net.h"

nodeid_t Net::add_node(Vector2 position, float angle, symbolid_t symbol) {
    portid_t ports = symbols[symbol].ports;
    return nodes.push(Node(position, {0, 0}, angle, 0, ports, symbol));
}
void Net::add_edge(Port from, Port to) {
    nodes[from.nodeid].ports[from.portid] = to;
    nodes[to.nodeid].ports[to.portid] = from;
}

Vector2 port_position(Port port, const Net &net) {
    const Node &node = net.nodes.at(port.nodeid);
    portid_t ports = net.symbols[node.symbol].ports;

    Vector2 offset;
    if (ports == 1)
        offset = {CRC_TRG_RATIO, 0};
    else if (port.portid == 0)
        offset = {1, 0};
    else
        offset = {-0.5f, 1 - 2.f * port.portid / ports};

    return Vector2Rotate(offset, node.angle) * NODE_RADIUS + node.position;
}

Vector2 port_direction(Port port, const Net &net) {
    float angle = net.nodes.at(port.nodeid).angle;

    if (port.portid == 0)
        return Vector2Rotate({1,0}, angle);
    return Vector2Rotate({-1,0}, angle);
}
