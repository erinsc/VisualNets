#pragma once

#include "sparse_array.h"
#include "raylib.h"
#include "raymath.h"
#include <deque>
#include <iostream>

enum Type { Main = 0, Left = 1, Right = 2, None = 3 };

struct Port {
    Type type;
    size_t value;

    operator int() const { return value; }
};

enum Symbol { Delta = 0, Alpha = 1, Lambda = 2, Gamma = 3, Epsilon = 4, Root = 5 };

struct Node {
    Vector2 position = {0,0};
    Vector2 velocity = {0,0};
    float angle = 0;
    float rotation = 0;

    Port main;
    Port left;
    Port right;

    Symbol symbol;

    bool   is_index()  const { return main.type == None; }
    size_t get_index() const { return main.value; }
    Node&  set_index(size_t index) {
        main.type = None;
        main.value = index;
        return *this;
    }
    static Node make_index(size_t index) {
        Node node;
        node.main = {None, index};
        return node;
    }
    Port& operator[](Type type) {
        switch(type) {
            case Main:  return main;
            case Left:  return left;
            case Right: return right;
            case None:  break;
        }
        return main;
    }
};

struct Net {
    SparseArray<Node> nodes;

    Net (size_t capacity) : nodes(capacity) {}

    Node&       operator[](size_t port)       { return nodes[port]; }
    const Node& operator[](size_t port) const { return nodes[port]; }
    Node        pop(size_t port)              { return nodes.pop(port); }
    size_t      push(const Node &node)        { return nodes.push(node); }
    size_t push(const Port &main, const Port &left,
                const Port &right, Symbol symbol) {
        return nodes.push({{}, {}, 0, 0, main, left, right, symbol});
    }

    void interact(size_t, size_t);
    void link(const Port &left, const Port &right);
};

using Rule = void(*)(size_t, size_t, Net&);

std::ostream& operator<<(std::ostream &os, const Net &net);
