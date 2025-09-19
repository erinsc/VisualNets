#pragma once

#include "raylib.h"
#include "raymath.h"
#include <map>
#include <set>
#include <vector>

using symbolid_t = unsigned short;
using nodeid_t = unsigned int;
using port_t = unsigned char;

struct Node {
    Vector2 position = {0, 0};
    Vector2 velocity = {0, 0};
    float angle = 0;
    float rotation = 0;

    symbolid_t symbol;
};

struct Symbol {
    Color color;
    port_t ports;
    wchar_t character;

    Symbol(Color color, port_t ports, wchar_t character)
    : color(color), ports(ports), character(character) {}
};

struct Edge {
    nodeid_t from;
    nodeid_t to;
    port_t from_port;
    port_t to_port;

    bool operator<(const Edge &other) const {
        return from < other.from;
    }
};

class Net {
    nodeid_t id_counter = 0;
public:
    std::vector<Symbol> symbols;
    std::map<nodeid_t, Node> nodes;
    std::multiset<Edge> edges;
};

