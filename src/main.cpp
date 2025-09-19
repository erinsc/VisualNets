#include "lambda_net.h"
#include "renderer.h"

int main() {
    int display = GetCurrentMonitor();
    Vector2 screensize = {(float)GetMonitorWidth(display), (float)GetMonitorHeight(display)};

    InitWindow(screensize.x, screensize.y, "LambdaNets");

    Camera2D camera = {};
    camera.zoom = 1.f;
    camera.offset = (Vector2){ screensize.x/2.0f, screensize.y/2.0f };
    camera.target = (Vector2){ screensize.x, screensize.y };

    SetTargetFPS(60);

    Net net;
    Renderer renderer;

    net.symbols.emplace_back(RED, 1, L'a');
    net.symbols.emplace_back(GREEN, 2, L'g');
    net.symbols.emplace_back(BLUE, 3, L'e');

    net.nodes[0] = {{200, 200}, {0,0}, 0, 0, 2};
    net.nodes[1] = {{200, 400}, {0,0}, 0, 0, 1};
    net.nodes[2] = {{200, 600}, {0,0}, 0, 0, 0};
    net.nodes[3] = {{600, 600}, {0,0}, 0, 0, 0};
    net.nodes[4] = {{400, 600}, {0,0}, 0, 0, 0};

    net.edges.insert({0, 3, 0, 0});
    net.edges.insert({0, 4, 1, 0});
    net.edges.insert({0, 1, 2, 0});
    net.edges.insert({1, 2, 1, 0});


    while (!WindowShouldClose()) {
        BeginDrawing();
        BeginMode2D(camera);

        ClearBackground(RAYWHITE);
        renderer.draw_net(net);

        EndMode2D();
        EndDrawing();

        net.nodes[0].angle += 0.01;
        net.nodes[1].angle += 0.02;
    }
}
