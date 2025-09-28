#pragma once

#include "lambda_net.h"
#include <variant>
#include <map>

using Reduction = std::vector<std::variant<symbolid_t, portid_t, bool>>;
using SymbolPair = std::pair<symbolid_t, symbolid_t>;

struct Reducer {
    std::map<SymbolPair, Reduction> reductions;
    void register_rule(const SymbolPair &symbol_pair, const Reduction &reduction);
    void reduce(nodeid_t leftid, nodeid_t rightid, Net &net);
};

/* Reduction algorithm:
 * Instructions come as a sequence of numbers, either symbols or port ids
 * 1. All aux ports of the two reducing nodes are added to a stack
 * 2. Instructions are read from the sequence
 *   If symbol, pop a port from the stack, add a new node
 *       then connect the main port of the agent to the popped port
 *       add new agents aux ports to the stack
 *   If port, pop and register a port into a lookup table
 *       if one is already registered, link them together into an edge
 *
 * Issues:
 *   Because the two nodes may be connected using more than one edge,
 *       its necesarry to access the aux ports live, not beforehand
 *   A node may connect to itself using an edge, and may form a loop
 *       loops must be specifically detected in the lookup table
 */

void Reducer::register_rule(const SymbolPair &symbol_pair, const Reduction &reduction) {
    reductions[symbol_pair] = reduction;
    size_t middle = 0;
    while(reduction[middle].index() != 2) ++middle;

    Reduction reversed = {};
    for(size_t i = middle+1; i < reduction.size(); ++i)
        reversed.push_back(reduction[i]);
    reversed.push_back(true);
    for(size_t i = 0; i < middle; ++i)
        reversed.push_back(reduction[i]);

    reductions[{symbol_pair.second, symbol_pair.first}] = reversed;
}

void Reducer::reduce(nodeid_t leftid, nodeid_t rightid, Net &net) {
    Node *left = &net.nodes.at(leftid);
    Node *right = &net.nodes.at(rightid);

    if (left->ports[0].nodeid != rightid or right->ports[0].nodeid != leftid)
        return;

    SymbolPair pair = {left->symbol, right->symbol};

    auto it = reductions.find(pair);
    if (it == reductions.end())
        return;

    //std::cout << "---- ---- " << (int)left->symbol << " " << (int)right->symbol << "\n";

    const Reduction &reduction = it->second;

    std::map<portid_t, Port> lookup_table;
    std::vector<Port> stack; // stack not pointing to the ports themselves, but to ports holding the ports we need

    for (portid_t p = right->ports.size()-1; p > 0; --p)
        stack.push_back({rightid, p});
    for (portid_t p = left->ports.size()-1; p > 0; --p)
        stack.push_back({leftid, p});

    nodeid_t refid = rightid;
    for (const auto &vr : reduction) {
        if (vr.index() == 2) {
            refid = leftid;
            continue;
        }
        /*
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
        std::cout << "\n";
        std::cout << "Stack: ";
        for (const Port &port : stack) {
            std::cout << "{" << (int)port.nodeid << " " << (int)port.portid << "}, ";
        }
        std::cout << "\n\n";
        */
        Port port = stack.back();
        stack.pop_back();

        if (vr.index() == 0) { // Add new node
            //std::cout << "Adding symbol " << (int)std::get<symbolid_t>(vr) << "\n";

            const Node &ref = net.nodes.at(refid);

            nodeid_t nnid = net.add_node(ref.position, ref.velocity,
                                         ref.angle, ref.rotation,
                                         std::get<symbolid_t>(vr));

            const Node &nn = net.nodes.at(nnid);
            for(portid_t p = nn.ports.size()-1; p > 0;--p) {
                stack.push_back({nnid, p});
            }
            net.add_edge(net.get_port(port), {nnid, 0});
            continue;
        }
                // Connect ports
        portid_t portid = std::get<portid_t>(vr);

        auto it = lookup_table.find(portid);
        if (it == lookup_table.end()) {
            //std::cout << "Inserting port " << (int)std::get<portid_t>(vr) << "\n";

            lookup_table[portid] = port;
            //net.get_port(port) = {(nodeid_t)-1, portid}; // TODO: figure out what this is for
            continue;
        }
        //std::cout << "Removing port " << (int)std::get<portid_t>(vr) << "\n";

        Port other = it->second;
        lookup_table.erase(it);

        net.add_edge(other, port);
    }
    net.nodes.pop(rightid);
    net.nodes.pop(leftid);
}
