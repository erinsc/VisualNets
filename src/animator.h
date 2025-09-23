#pragma once

#include "lambda_net.h"
#include <iostream>

class Animator {
public:
    float force_noncut = 4;
    float force_cut = 32;

    void step_net(Net &net);
private:
    void step_node(Node &node);
    void push_at_point(Node &node, Vector2 force, Vector2 point);
    void pull_edge(const Edge &edge, Net &net);
    void push_away_nodes(Node &left, Node &right);
};
