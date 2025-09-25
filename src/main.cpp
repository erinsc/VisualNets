#include "lambda_net.h"
#include "renderer.h"

int main() {
    int display = GetCurrentMonitor();
    Vector2 screensize = {(float)GetMonitorWidth(display), (float)GetMonitorHeight(display)};

    InitWindow(screensize.x, screensize.y, "LambdaNets");

    SetTargetFPS(60);

    Net net;
    Renderer renderer(screensize);

    net.symbols.emplace_back(SKYBLUE, 1, L'e');
    net.symbols.emplace_back(GREEN, 3, L'g');
    net.symbols.emplace_back(RED, 4, L'd');

    net.add_node({500, 501}, 0, 0);
    net.add_node({501, 500}, 0, 0);
    net.add_node({502, 500}, 0, 1);
    net.add_node({503, 500}, 0, 0);
    net.add_node({504, 500}, 0, 2);
    net.add_node({505, 500}, 0, 1);
    net.add_node({506, 500}, 0, 0);
    net.add_node({507, 500}, 0, 0);
    net.add_node({508, 500}, 0, 0);

    net.add_edge((Port){0, 0}, (Port){2, 1});
    net.add_edge((Port){3, 0}, (Port){2, 2});
    net.add_edge((Port){1, 0}, (Port){4, 2});
    net.add_edge((Port){2, 0}, (Port){4, 0});
    net.add_edge((Port){5, 0}, (Port){4, 1});
    net.add_edge((Port){6, 0}, (Port){4, 3});

    net.add_edge((Port){7, 0}, (Port){5, 1});
    net.add_edge((Port){8, 0}, (Port){5, 2});

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
    }
}
