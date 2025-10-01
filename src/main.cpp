#include "net.h"
#include "renderer.h"
#include "reducer.h"
#include "lambdanets/parser.h"

int main() {
    int display = GetCurrentMonitor();
    Vector2 screensize = {(float)GetMonitorWidth(display), (float)GetMonitorHeight(display)};

    InitWindow(screensize.x, screensize.y, "LambdaNets");

    SetTargetFPS(60);

    bool simulate = true;
    bool reduce = false;

    Net net;
    Renderer renderer(screensize);
    Reducer reducer;

    net.symbols.emplace_back(GRAY, 1, " ");
    net.symbols.emplace_back(ORANGE, 3, "A");
    net.symbols.emplace_back(GREEN, 3, "G");
    net.symbols.emplace_back(RED, 3, "D");
    net.symbols.emplace_back(SKYBLUE, 1, "E");
    net.symbols.emplace_back(YELLOW, 3, "L");

    Reduction DD = {{(portid_t)1, (portid_t)2},
                    {(portid_t)1, (portid_t)2}};
    Reduction DA = {{(symbolid_t)Alpha, (portid_t)1, (portid_t)2,
                     (symbolid_t)Alpha, (portid_t)3, (portid_t)4},
                    {(symbolid_t)Delta, (portid_t)1, (portid_t)3,
                     (symbolid_t)Delta, (portid_t)2, (portid_t)4}};
    Reduction DL = {{(symbolid_t)Lambda,(portid_t)1, (portid_t)2,
                     (symbolid_t)Lambda,(portid_t)3, (portid_t)4},
                    {(symbolid_t)Delta, (portid_t)1, (portid_t)3,
                     (symbolid_t)Delta, (portid_t)2, (portid_t)4}};
    Reduction DG = {{(symbolid_t)Gamma, (portid_t)1, (portid_t)2,
                     (symbolid_t)Gamma, (portid_t)3, (portid_t)4},
                    {(symbolid_t)Delta, (portid_t)1, (portid_t)3,
                     (symbolid_t)Delta, (portid_t)2, (portid_t)4}};
    Reduction AG = {{(symbolid_t)Gamma, (portid_t)1, (portid_t)2,
                     (symbolid_t)Gamma, (portid_t)3, (portid_t)4},
                    {(symbolid_t)Delta, (portid_t)1, (portid_t)3,
                     (symbolid_t)Delta, (portid_t)2, (portid_t)4}};
    Reduction LG = {{(portid_t)1, (symbolid_t)Gamma, (symbolid_t)Gamma,
                     (portid_t)2, (portid_t)3, (portid_t)1},
                    {(portid_t)2, (portid_t)3}};
    Reduction GG = {{(portid_t)1, (portid_t)2},
                    {(portid_t)1, (portid_t)2}};
    Reduction DE = {{(symbolid_t)Epsilon, (symbolid_t)Epsilon},{}};
    Reduction AE = {{(symbolid_t)Epsilon, (symbolid_t)Epsilon},{}};
    Reduction LE = {{(symbolid_t)Epsilon, (symbolid_t)Epsilon},{}};
    Reduction GE = {{(symbolid_t)Epsilon, (symbolid_t)Epsilon},{}};
    Reduction EE = {{},{}};

    reducer.register_rule({Delta,Delta},    DD);
    reducer.register_rule({Delta,Alpha},    DA);
    reducer.register_rule({Delta,Lambda},   DL);
    reducer.register_rule({Delta,Gamma},    DG);
    reducer.register_rule({Alpha,Gamma},    AG);
    reducer.register_rule({Lambda,Gamma},   LG);
    reducer.register_rule({Gamma,Gamma},    GG);
    reducer.register_rule({Delta,Epsilon},  DE);
    reducer.register_rule({Alpha,Epsilon},  AE);
    reducer.register_rule({Lambda,Epsilon}, LE);
    reducer.register_rule({Gamma,Epsilon},  GE);
    reducer.register_rule({Epsilon,Epsilon},EE);

    term_map tm = {};

    //std::string s = "(\\n.n (\\x.(\\tf.f)) (\\tf.t))(\\xf.f(f(f(f(f(fx))))))";
    std::string s = "(\\fx.f(fx))(\\x.xx)";
    nodeid_t eps = net.add_node({0,0},0,Root);

    Port tree = parse_term(s, tm, net);
    net.nodes[tree.nodeid].ports[tree.portid] = {(nodeid_t)-1, (portid_t)-1};

    net.add_edge(tree, {eps, 0});

    auto it = net.nodes.begin();
    net.nodes.at(*(it)).position += {0,1};
    net.nodes.at(*(++it)).position += {1,0};

    while (!WindowShouldClose()) {
        simulate ^= IsKeyPressed(KEY_ONE);
        reduce ^= IsKeyPressed(KEY_TWO);

        BeginDrawing();
        BeginMode2D(renderer.camera);

        ClearBackground(RAYWHITE);
        renderer.draw_net(net);

        EndMode2D();
        EndDrawing();

        if (simulate)
            renderer.step_net(net);

        renderer.step_camera();
        renderer.move_node(net);

        renderer.push_mult = reduce ? 8 : 1 ;
        float distance = simulate ? NODE_RADIUS * 2 : -1;
        if (reduce)
            reducer.reduce_all(distance, net);
    }
}
