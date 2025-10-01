#pragma once

#include "raylib.h"
#include "raymath.h"

#include <vector>
#include "sparse_vector.h"

using symbolid_t = unsigned short;
using nodeid_t = unsigned int;
using portid_t = unsigned char;

constexpr float TODEG = 180 / PI;
constexpr float CRC_TRG_RATIO = 0.64;

constexpr float NODE_RADIUS = 32;
constexpr float NODE_MASS = NODE_RADIUS * NODE_RADIUS;
constexpr float NODE_INERTIA = NODE_MASS * NODE_RADIUS * NODE_RADIUS / 4;

constexpr float EDGE_WIDTH = NODE_RADIUS / 4;

constexpr float INFLUENCE_RADIUS = NODE_RADIUS * 8;
constexpr float PUSH_FORCE = NODE_RADIUS;

struct Port {
    nodeid_t nodeid = (nodeid_t)-1;
    portid_t portid = (portid_t)-1;
};

// Agent in an interaction net
struct Node {
    union {
        struct {
            Vector2 position;
            Vector2 velocity;
            float angle;
            float rotation; // Angular velocity
        };
        size_t index; // Index in SparseVector
    };
    std::vector<Port> ports;
    symbolid_t symbol;

    // Full constructor
    Node(Vector2 position, Vector2 velocity, float angle, float rotation, portid_t ports, symbolid_t symbol)
    : position(position), velocity(velocity), angle(angle), rotation(rotation), ports(ports), symbol(symbol) {}

    // Functions for SparseVector
    bool is_index() const { return symbol == (symbolid_t)-1; }
    size_t get_index() const { return index; }
    Node& set_index(size_t index) {
        symbol = (symbolid_t)-1;
        this->index = index;
        return *this;
    }
    static Node make_index(size_t index) {
        return Node(index, (symbolid_t)-1);
    }
    Node() {} // TODO: Get rid of this
private:
    // Constructor for the ::make_index function
    Node(size_t index, symbolid_t symbol) : index(index), symbol(symbol) {}
};

// Properties of a symbol - type of agent
struct Symbol {
    Color color;
    portid_t ports; // Number of ports including main port
    const char *text;
    Vector2 text_size;

    Symbol(Color color, portid_t ports, const char *text)
    : color(color), ports(ports), text(text) {
        Font font = GetFontDefault();
        text_size = MeasureTextEx(font, text, NODE_RADIUS, 0) / 2;
    }
};

class Net {
public:
    std::vector<Symbol> symbols;
    SparseVector<Node> nodes;

    nodeid_t add_node(Vector2 position, float angle, symbolid_t symbol);
    nodeid_t add_node(Vector2 position, Vector2 velocity, float angle, float rotation, symbolid_t symbol);
    void add_edge(Port from, Port to);

    Port&       get_port(const Port &port);
    const Port& get_port(const Port &port) const;
};

Vector2 port_position(Port port, const Net &net);
Vector2 port_direction(Port port, const Net &net);
