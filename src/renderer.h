#pragma once

#include "lambda_net.h"

#include <iostream>

constexpr float NODE_SIZE = 32;
constexpr float EDGE_SIZE = 8;
constexpr float TODEG = 180 / PI;

constexpr float CRC_TRG_RATIO = 0.85;

class Renderer {
public:

    void draw_net(const Net &net) const {
        for (auto &edge : net.edges) {
            draw_edge(edge, net);
        }

        for (auto &[id, node] : net.nodes) {
            draw_node(node, net);
        }
    }

private:
    void draw_node(const Node &node, const Net &net) const {

        Color color = net.symbols.at(node.symbol).color;
        port_t ports = net.symbols.at(node.symbol).ports;
        if (ports <= 1) {
            DrawCircleV(node.position, NODE_SIZE*CRC_TRG_RATIO+EDGE_SIZE, Fade(BLACK,0.3));
            DrawCircleV(node.position, NODE_SIZE*CRC_TRG_RATIO, Fade(color,0.3));
        }
        else {
            DrawPoly(node.position, 3, NODE_SIZE+EDGE_SIZE*2, node.angle*TODEG, Fade(BLACK,0.3));
            DrawPoly(node.position, 3, NODE_SIZE, node.angle*TODEG, Fade(color,0.3));
        }
    }
    void draw_edge(const Edge &edge, const Net &net) const {
        const Node &from = net.nodes.at(edge.from);
        const Node &to = net.nodes.at(edge.to);

        auto [A, A_dir] = get_port_offsets(from, edge.from_port, net);
        auto [D, D_dir] = get_port_offsets(to, edge.to_port, net);

        Vector2 diff = A - D;

        float a = Vector2DotProduct(diff, Vector2Rotate({1,0}, from.angle)) / 4 + Vector2Length(diff);
        float d = -Vector2DotProduct(diff, Vector2Rotate({1,0}, to.angle)) / 4 + Vector2Length(diff);

        Vector2 B = A + A_dir * a;
        Vector2 C = D + D_dir * d;

        DrawSplineSegmentBezierCubic(A, B, C, D, EDGE_SIZE, BLACK);
    }

    std::pair<Vector2, Vector2> get_port_offsets(const Node &node, port_t port, const Net &net) const {
        port_t ports = net.symbols[node.symbol].ports;

        Vector2 offset;
        float angle;
        if (port == 0) {
            offset = {1, 0};
            angle = node.angle;
        }
        else {
            offset = {1.f/2, 2.f * port / ports - 1};

            angle = node.angle + PI;
        }
        Vector2 point = Vector2Rotate(offset, angle) * NODE_SIZE + node.position;
        Vector2 dir = Vector2Rotate({1,0}, angle);

        return {point, dir};
    }
};
