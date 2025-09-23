#pragma once

#include "lambda_net.h"
#include <iostream>

using RedRule = std::vector<std::variant<portid_t, symbolid_t, bool>>;

class Reducer {
public:
    std::map<std::pair<symbolid_t, symbolid_t>, RedRule> reductions;

    void reduce_net(Net &net) {
        for (auto it = net.edges.begin(); it != net.edges.end(); ++it) {
            const Edge &edge = *it;

            if (not edge.is_cut())
                continue;

            const Node &from = net.nodes[edge.from.nodeid];
            const Node &to = net.nodes[edge.to.nodeid];

            auto [A, _1] = get_port_offsets(from, edge.from.portid, net);
            auto [D, _2] = get_port_offsets(to, edge.to.portid, net);

            float distance = Vector2Distance(A,D);
            if (distance >= Node::radius)
                continue;

            reduce_edge(it, net);
            return;
        }
    }
private:
    void reduce_edge(std::multiset<Edge>::iterator &edgeit, Net &net) {
        const Edge edge = *edgeit;

        if (not edge.is_cut())
            return;

        const Node &left = net.nodes[edge.from.nodeid];
        symbolid_t leftt = left.symbol;
        const Node &right = net.nodes[edge.to.nodeid];
        symbolid_t rightt = right.symbol;
        auto it = reductions.find({leftt, rightt});

        std::cout << edge.from.nodeid << " " << edge.to.nodeid << "\n";

        if (it == reductions.end()) {
            it = reductions.find({rightt, leftt});
            if (it == reductions.end()) {
                std::cerr << "The reduction of " << net.symbols[leftt].character;
                std::cerr << " and " << net.symbols[rightt].character;
                std::cerr << " is not defined\n";
                return;
            }
        }
        net.edges.erase(edgeit);

        const auto &[nd, rule] = *it;

        int total_ports = net.symbols[leftt].ports + net.symbols[rightt].ports - 2;
        int left_ports = net.symbols[leftt].ports;

        std::vector<Port> portstack(total_ports);

        for (Port &port : portstack) {
            port = {420, 69};
        }

        for (auto it = net.edges.begin(); it != net.edges.end();) {
            const Edge &tested = *it;

            if (tested.from.nodeid == edge.from.nodeid) {
                std::cout << "A " << tested.from.portid-1 << " ";
                std::cout << tested.from.nodeid << " " << (int)tested.from.portid << "\n";

                portstack.at(tested.from.portid-1) = tested.to;
                net.edges.erase(it);
            }
            else if (tested.to.nodeid == edge.from.nodeid) {
                std::cout << "B " << tested.to.portid-1 << " ";
                std::cout << tested.to.nodeid << " " << (int)tested.to.portid << "\n";

                portstack.at(tested.to.portid-1) = tested.from;
                net.edges.erase(it);
            }
            else if (tested.from.nodeid == edge.to.nodeid) {
                std::cout << "C " << tested.to.portid+left_ports-2 << " ";
                std::cout << tested.from.nodeid << " " << (int)tested.from.portid << "\n";

                portstack.at(tested.to.portid+left_ports-2) = tested.to;
                net.edges.erase(it);
            }
            else if (tested.to.nodeid == edge.to.nodeid) {
                std::cout << "D " << tested.from.portid+left_ports-2 << " ";
                std::cout << tested.to.nodeid << " " << (int)tested.to.portid << "\n";

                portstack.at(tested.from.portid+left_ports-2) = tested.from;
                net.edges.erase(it);
            }
            else {

            }
            ++it;
        }
        for (const Port &port : portstack) {
            std::cout << port.nodeid << " " << (int)port.portid << "\n";
        }

        //for (const Port &port : portstack) {
        //    std::cout << port.nodeid << "  " << (int)port.portid << "\n";
        //}

        std::map<portid_t, Port> lookup;

        const Node *current_node = &right;
        for (const auto &vr : rule) {
            std::cout << "----------------\n";
            if (vr.index() == 2) {
                current_node = &left;
                continue;
            }

            Port port = portstack.back();
            portstack.pop_back();

            if (vr.index() == 1) {
                std::cout << "ADDING NODE WITH SYMBOL " << std::get<symbolid_t>(vr) << "\n";
                nodeid_t id = net.insert_node({current_node->position, current_node->velocity,
                                 current_node->angle, current_node->rotation,
                                 std::get<symbolid_t>(vr)});

                std::cout << "    ID = " << id << "\n";
                std::cout << "    OTHER " << port.nodeid << " " << (int)port.portid << "\n";
                net.edges.insert({port, {id, 0}});
            }
            else {
                portid_t current = std::get<portid_t>(vr);
                if (lookup.find(current) != lookup.end()) {
                    std::cout << "CONNECTING " << port.nodeid << " " << (int)port.portid;
                    std::cout << " AND " << lookup.at(current).nodeid << " " << lookup.at(current).portid;
                    net.edges.insert({port, lookup.at(current)});
                }
                else {
                    std::cout << "LOOKUP ADDING " << port.nodeid << " " << (int)port.portid << "\n";
                    lookup[current] = port;
                }
            }
        }

        net.nodes.erase(edge.from.nodeid);
        net.nodes.erase(edge.to.nodeid);
    }
};
