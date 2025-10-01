#include "reducer.h"

void Reducer::register_rule(const SymbolPair &pair, const Reduction &reduction) {
    //std::cout << pair.first << " " << pair.second << "\n";
    reductions[pair] = reduction;
    Reduction reversed = {reduction.right, reduction.left};
    reductions[{pair.second, pair.first}] = reversed;
}

/* Reduction algorithm:
 * Instructions come as a sequence of numbers, either symbols or port ids
 * 1. All aux ports of the two reducing nodes are added to a stack
 * 2. Instructions are read from the sequence
 *   If symbol, pop a port from the stack, add a new node
 *       then connect the main port of the agent to the popped port
 *       add new agents aux ports to the stack
 *   If port, pop and register a port into a lookup table
 *       if one is already registered, link them together into an edge
 */
void Reducer::reduce(nodeid_t leftid, nodeid_t rightid, Net &net) const {
    SymbolPair pair = {net.nodes.at(leftid).symbol, net.nodes.at(rightid).symbol};

    auto it = reductions.find(pair);
    std::cout << pair.first << " " << pair.second << "\n";
    if (it == reductions.end()) {
        return;
    }
    const Reduction &reduction = it->second;

    std::map<portid_t, Port> lookup_table;
    std::cout << "Starting reduction\n";
    reconnect_ports(leftid, rightid, reduction.left, lookup_table, net);
    reconnect_ports(rightid, leftid, reduction.right, lookup_table, net);

    net.nodes.pop(rightid);
    net.nodes.pop(leftid);
    std::cout << "-------------\n";
    for (nodeid_t id : net.nodes) {
        std::cout << id << " " << net.nodes.at(id).symbol << ": ";
        for (Port port : net.nodes.at(id).ports) {
            std::cout << "{" << (int)port.nodeid << " " << (int)port.portid << "}, ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    for (auto [id, port] : lookup_table) {
        std::cout << (int)id << " -> ";
        std::cout << "{" << (int)port.nodeid << " " << (int)port.portid << "}, ";
        std::cout << "\n";
    }
}

void Reducer::reconnect_ports(nodeid_t id, nodeid_t refid, const RuleList &rulelist, std::map<portid_t, Port> &lookup_table, Net &net) const {
    std::vector<Port> stack; // stack not pointing to the ports themselves, but to ports holding the ports we need

    const Node &node = net.nodes.at(id);
    const Node &ref = net.nodes.at(refid);

    for (portid_t p = node.ports.size()-1; p > 0; --p)
        stack.push_back({id, p});

    for (const auto &vr : rulelist) {

        for (nodeid_t id : net.nodes) {
            std::cout << id << " " << net.nodes.at(id).symbol << ": ";
            for (Port port : net.nodes.at(id).ports) {
                std::cout << "[" << (int)port.nodeid << " " << (int)port.portid << "] ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        for (auto [id, port] : lookup_table) {
            std::cout << (int)id << " -> ";
            std::cout << "[" << (int)port.nodeid << " " << (int)port.portid << "] ";
            std::cout << "\n";
        }
        std::cout << "\n";
        std::cout << "Stack: ";
        for (const Port &port : stack) {
            std::cout << "[" << (int)port.nodeid << " " << (int)port.portid << "] ";
        }
        std::cout << "\n\n";


        Port port = stack.back();
        stack.pop_back();

        if (vr.index() == 0) { // Add new node
            std::cout << "Adding node: " << (int)std::get<symbolid_t>(vr) << "\n";
            nodeid_t nnid = net.add_node(ref.position, ref.velocity,
                                         ref.angle, ref.rotation,
                                         std::get<symbolid_t>(vr));

            Node &nn = net.nodes.at(nnid);
            for(portid_t p = nn.ports.size()-1; p > 0;--p) {
                stack.push_back({nnid, p});
                nn.ports[p] = {nnid, p};
            }
            net.add_edge(net.get_port(port), {nnid, 0});
            continue;
        }
        // Connect ports
        portid_t portid = std::get<portid_t>(vr);

        auto it = lookup_table.find(portid);
        if (it == lookup_table.end()) {
            std::cout << "Inserting portid: " << (int)std::get<portid_t>(vr) << "\n";
            lookup_table[portid] = port;
            continue;
        }
        std::cout << "Joining portid: " << (int)std::get<portid_t>(vr) << "\n";
        net.add_edge(net.get_port(it->second), net.get_port(port));
        lookup_table.erase(it);
    }
}
void Reducer::reduce_all(float distance, Net &net) const {
    for (nodeid_t id : net.nodes) {
        const Node &node = net.nodes.at(id);
        nodeid_t otherid = node.ports[0].nodeid;
        const Node &other = net.nodes.at(otherid);
        nodeid_t ooid = other.ports[0].nodeid;

        if (ooid != id or id == 0 or ooid == 0)
            continue;

        float dist = Vector2Distance(node.position, other.position);
        if (dist < distance or distance < 0) {
            reduce(id, otherid, net);
        }
    }
}
