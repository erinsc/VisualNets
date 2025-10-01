#include "net.h"

nodeid_t Net::add_node(Vector2 position, float angle, symbolid_t symbol) {
    portid_t ports = symbols[symbol].ports;
    return nodes.push(Node(position, {0, 0}, angle, 0, ports, symbol));
}
nodeid_t Net::add_node(Vector2 position, Vector2 velocity, float angle, float rotation, symbolid_t symbol) {
    portid_t ports = symbols[symbol].ports;
    return nodes.push(Node(position, velocity, angle, rotation, ports, symbol));
}
void Net::add_edge(Port from, Port to) {
    get_port(from) = to;
    get_port(to) = from;
}
Port& Net::get_port(const Port &port) {
    return nodes.at(port.nodeid).ports.at(port.portid);
}
const Port& Net::get_port(const Port &port) const {
    return nodes.at(port.nodeid).ports.at(port.portid);
}
Vector2 port_position(Port port, const Net &net) {
    const Node &node = net.nodes.at(port.nodeid);
    portid_t ports = net.symbols[node.symbol].ports;

    Vector2 offset;
    if (ports == 1)            offset = {CRC_TRG_RATIO, 0};
    else if (port.portid == 0) offset = {1, 0};
    else                       offset = {-0.5f, 2.f * port.portid / ports - 1};

    return Vector2Rotate(offset, node.angle) * NODE_RADIUS + node.position;
}

Vector2 port_direction(Port port, const Net &net) {
    float angle = net.nodes.at(port.nodeid).angle;

    if (port.portid == 0)
        return Vector2Rotate({1,0}, angle);
    return Vector2Rotate({-1,0}, angle);
}
