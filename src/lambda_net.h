#pragma once

#include "raylib.h"
#include "raymath.h"
#include <map>
#include <set>
#include <vector>
#include <variant>
#include <string>

using symbolid_t = unsigned short;
using nodeid_t = unsigned int;
using portid_t = unsigned char;

constexpr float TODEG = 180 / PI;
constexpr float CRC_TRG_RATIO = 0.64;

struct Node {
    Vector2 position = {0, 0};
    Vector2 velocity = {0, 0};
    float angle = 0;
    float rotation = 0;
    symbolid_t symbol;

    static float mass;
    static float radius;
    static float inertia;
};

inline float Node::radius = 32;
inline float Node::mass = Node::radius * Node::radius;
inline float Node::inertia = Node::mass * Node::radius * Node::radius / 4;

struct Symbol {
    Color color;
    portid_t ports;
    char character;

    Symbol(Color color, portid_t ports, char character)
    : color(color), ports(ports), character(character) {}
};

struct Port {
    nodeid_t nodeid;
    portid_t portid;
};

struct Edge {
    Port from;
    Port to;

    bool operator<(const Edge &other) const {
        if (from.nodeid == other.from.nodeid)
            return to.nodeid < other.to.nodeid;
        return from.nodeid < other.from.nodeid;
    }
    bool is_cut() const {
        return from.portid == 0 and to.portid == 0;
    }

    static float width;
};

inline float Edge::width = 8;

class Net {
    nodeid_t id_counter = 0;
public:
    std::vector<Symbol> symbols;
    std::map<nodeid_t, Node> nodes;
    std::multiset<Edge> edges;

    nodeid_t insert_node(const Node &node);
    template <typename ...Ts>
    nodeid_t emplace_node(Ts &&...args);
};

std::pair<Vector2, Vector2> get_port_offsets(const Node &node, portid_t port, const Net &net);
