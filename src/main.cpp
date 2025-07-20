#include "lambda_net.h"
#include "graphics.h"
#include "parser.h"

bool step(Net &net) {
    for (const auto [i, node] : net.nodes) {
        Port main = node.main;
        Port other_main = net[main].main;

        if (main.type == Main and other_main.value == i) {
            std::cout << i << "\n";
            net.interact(main, other_main);
            return true;
        }
    }
    return false;
}

int main() {
    int display = GetCurrentMonitor();
    Vector2 screensize = {(float)GetMonitorWidth(display), (float)GetMonitorHeight(display)};

    InitWindow(screensize.x, screensize.y, "LambdaNets");

    bool running = false;
    bool paused = true;

    Camera2D camera = {};
    camera.zoom = 1.f;
    camera.offset = (Vector2){ screensize.x/2.0f, screensize.y/2.0f };
    camera.target = (Vector2){ screensize.x, screensize.y };

    SetTargetFPS(60);

    Net net(4096);

    term_map tm = {
        {"I", "\\x.x"},
        {"K", "\\xy.x"},
        {"S", "\\xyz.xz(yz)"},
        {"B", "\\xyz.x(yz)"},
        {"C", "\\xyz.xzy"},
        {"W", "\\xy.xyy"},

        {"T", "\\tf.t"},
        {"F", "\\tf.f"},
        {"~", "\\a.a F T"},
        {"|", "\\a.aa"},
        {"&", "\\ab.aba"},
        {"^", "\\ab.a(~ b)b"},

        {"0", "\\fx.x"},
        {"1", "\\fx.fx"},
        {"2", "\\fx.f(fx)"},
        {"3", "\\fx.f(f(fx))"},
        {"4", "\\fx.f(f(f(fx)))"},
        {"5", "\\fx.f(f(f(f(fx))))"},
        {"6", "\\fx.f(f(f(f(f(fx)))))"},
        {"++", "\\nfx.nf(fx)"},
        {"+", "\\ab.a ++ b"},
        {"*", "\\abf.a(bf)"},
        {"**", "\\ab.ba"},
        {"--", "\\nfx.n((\\fgh.h(gf))f)((\\xu.x)x)(\\u.u)"},
        {"-", "\\ab.a -- b"},
        {"=0", "\\n.n (\\x.F) T"},
        {"<=", "\\nm.=0(- n m)"},
        {"<", "\\nm.=0(- (++ n) m)"},
        {"==", "\\nm.&(<= m n)(<= n m)"},
        {"!", "@n.n(@p.p(@lrq.q((@lfx.lf(fx))l)((@lrf.l(rf))lr)))(@p.p(@x.x)(@y.y))(@tf.f)"},
        //{"", ""},
    };
    // 4! == 4 * 6
    std::string s = "== (* 4 4) (+ 1 (* 3 (+ 2 3)))";

    size_t root = net.push({{0, 0}, {0, 0}, -PI/2, 0, {Right, 0}, {Left, 1}, {Main, 0}, Root});
    Port tree = parse_term(s, tm, net);
    net.link({Left, root}, tree);

    for (auto [i, node] : net.nodes) {
        Vector2 randpos = {(float)GetRandomValue(-100, 100),
                           (float)GetRandomValue(-100, 100)};
        node.position = randpos * 10;
    }
    net[root].position = {0,0};

    std::cout << net << "\n\n";

    while (!WindowShouldClose()) {
        running ^= IsKeyPressed(KEY_SPACE);
        paused = paused and not running;

        if (!paused)
            physics_step_net(net, running);

        control_camera(camera);

        BeginDrawing();
        BeginMode2D(camera);

        ClearBackground(RAYWHITE);
        draw_net(net);

        EndMode2D();
        EndDrawing();
    }
}
