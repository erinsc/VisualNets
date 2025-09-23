#include "lambda_net.h"
#include "renderer.h"
#include "animator.h"
#include "reducer.h"

int main() {
    int display = GetCurrentMonitor();
    Vector2 screensize = {(float)GetMonitorWidth(display), (float)GetMonitorHeight(display)};

    InitWindow(screensize.x, screensize.y, "LambdaNets");

    SetTargetFPS(60);

    Net net;
    Renderer renderer(screensize);
    Animator animator;
    Reducer reducer;

    net.symbols.emplace_back(SKYBLUE, 1, L'e');
    net.symbols.emplace_back(GREEN, 3, L'g');
    net.symbols.emplace_back(RED, 3, L'd');

    net.insert_node({{200, 200}, {0,0}, 0, 0, 0});
    net.insert_node({{200, 600}, {0,0}, 0, 0, 0});
    net.insert_node({{600, 200}, {0,0}, 0, 0, 1});
    net.insert_node({{600, 600}, {0,0}, 0, 0, 2});

    net.edges.insert({(Port){0, 0}, (Port){2, 1}});
    net.edges.insert({(Port){1, 0}, (Port){3, 2}});
    net.edges.insert({(Port){2, 0}, (Port){3, 0}});
    net.edges.insert({(Port){2, 2}, (Port){3, 1}});


    RedRule ee = {}; // E() >< E()
    RedRule eg = {false, symbolid_t{0}, symbolid_t{0}}; // E() >< G(E(), E())
    RedRule ed = {false, symbolid_t{0}, symbolid_t{0}}; // E() >< D(E(), E())

    RedRule gg = {portid_t{1}, portid_t{2}, false, portid_t{2}, portid_t{1}}; // G(1,2) >< G(2,1)

    // G(D(1,2), D(3,4)) >< D(G(1,3), G(2,4))
    RedRule gd = {
        symbolid_t{2}, portid_t{1}, portid_t{2},
        symbolid_t{2}, portid_t{3}, portid_t{4},
        false,
        symbolid_t{1}, portid_t{1}, portid_t{3},
        symbolid_t{1}, portid_t{2}, portid_t{4}
    };

    RedRule dd = {portid_t{1}, portid_t{2}, false, portid_t{1}, portid_t{2}}; // G(1,2) >< G(1,2)

    reducer.reductions[{0, 0}] = ee;
    reducer.reductions[{0, 1}] = eg;
    reducer.reductions[{0, 2}] = ed;

    reducer.reductions[{1, 1}] = gg;
    reducer.reductions[{1, 2}] = gd;
    reducer.reductions[{2, 2}] = dd;

    while (!WindowShouldClose()) {
        renderer.control_camera();

        BeginDrawing();
        BeginMode2D(renderer.camera);

        ClearBackground(RAYWHITE);
        renderer.draw_net(net);

        EndMode2D();
        EndDrawing();

        animator.step_net(net);

        //reducer.reduce_net(net);

    }
}
