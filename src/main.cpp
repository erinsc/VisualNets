#include "lambda_net.h"
#include "renderer.h"
#include "reducer.h"

int main() {
    int display = GetCurrentMonitor();
    Vector2 screensize = {(float)GetMonitorWidth(display), (float)GetMonitorHeight(display)};

    InitWindow(screensize.x, screensize.y, "LambdaNets");

    SetTargetFPS(60);

    Net net;
    Renderer renderer(screensize);
    Reducer reducer;

    net.symbols.emplace_back(SKYBLUE, 1, L'e');
    net.symbols.emplace_back(GREEN, 3, L'g');
    net.symbols.emplace_back(RED, 3, L'd');

    Reduction BlueBlue =   {false};
    Reduction BlueGreen =  {false,         (symbolid_t)0, (symbolid_t)0};
    Reduction BlueRed =    {false,         (symbolid_t)0, (symbolid_t)0};
    Reduction GreenGreen = {(portid_t)1,   (portid_t)2,
                            false,
                            (portid_t)2,   (portid_t)1};
    Reduction RedRed =     {(portid_t)1,   (portid_t)2,
                            false,
                            (portid_t)1,   (portid_t)2};
    Reduction GreenRed =   {(symbolid_t)1, (portid_t)1, (portid_t)2,
                            (symbolid_t)1, (portid_t)3, (portid_t)4,
                            false,
                            (symbolid_t)2, (portid_t)1, (portid_t)3,
                            (symbolid_t)2, (portid_t)2, (portid_t)4};

    reducer.register_rule({0,0}, BlueBlue);
    reducer.register_rule({0,1}, BlueGreen);
    reducer.register_rule({0,2}, BlueRed);
    reducer.register_rule({1,1}, GreenGreen);
    reducer.register_rule({1,2}, GreenRed);
    reducer.register_rule({2,2}, RedRed);

    /*// INFINITE THINGY
    net.add_node({500, 501}, 0, 0);
    net.add_node({501, 500}, 0, 0);
    net.add_node({502, 500}, 0, 2);
    net.add_node({503, 500}, 0, 1);

    net.add_edge((Port){2, 0}, (Port){3, 0});
    net.add_edge((Port){0, 0}, (Port){2, 1});
    net.add_edge((Port){1, 0}, (Port){3, 2});
    net.add_edge((Port){3, 1}, (Port){2, 2});
    */
    // Y FORK
    net.add_node({500, 501}, 0, 0);
    net.add_node({501, 500}, 0, 0);
    net.add_node({502, 500}, 0, 0);
    net.add_node({503, 500}, 0, 2);

    net.add_edge((Port){0, 0}, (Port){3, 0});
    net.add_edge((Port){1, 0}, (Port){3, 1});
    net.add_edge((Port){2, 0}, (Port){3, 2});

    // X FORK
    net.add_node({500, 501}, 0, 0);
    net.add_node({501, 500}, 0, 0);
    net.add_node({502, 500}, 0, 2);
    net.add_node({503, 500}, 0, 1);
    net.add_node({504, 500}, 0, 0);
    net.add_node({505, 500}, 0, 0);

    net.add_edge((Port){4, 0}, (Port){6, 1});
    net.add_edge((Port){5, 0}, (Port){6, 2});
    net.add_edge((Port){7, 0}, (Port){6, 0});
    net.add_edge((Port){7, 2}, (Port){8, 0});
    net.add_edge((Port){7, 1}, (Port){9, 0});

    // LOOP1
    net.add_node({500, 501}, 0, 0);
    net.add_node({501, 500}, 0, 2);

    net.add_edge((Port){10, 0}, (Port){11, 0});
    net.add_edge((Port){11, 1}, (Port){11, 2});

    // LOOP2
    net.add_node({500, 501}, 0, 2);
    net.add_node({501, 500}, 0, 2);

    net.add_edge((Port){12, 0}, (Port){13, 0});
    net.add_edge((Port){12, 1}, (Port){12, 2});
    net.add_edge((Port){13, 1}, (Port){13, 2});

    while (!WindowShouldClose()) {
        renderer.step_camera();

        BeginDrawing();
        BeginMode2D(renderer.camera);

        ClearBackground(RAYWHITE);
        renderer.draw_net(net);

        EndMode2D();
        EndDrawing();

        renderer.step_net(net);
        renderer.move_node(net);

        if (IsKeyPressed(KEY_SPACE)) {
            for (nodeid_t id : net.nodes) {
                nodeid_t otherid = net.nodes.at(id).ports[0].nodeid;
                const Node &other = net.nodes.at(otherid);
                if (other.ports[0].nodeid == id) {
                    reducer.reduce(id, otherid, net);
                    break;
                }

            }
        }
    }
}
