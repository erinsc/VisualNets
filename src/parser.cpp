#include "parser.h"

/* Lambda Calculus to LambdaNets:
  Abstraction (if no occurences): Gamma(Epsilon, <body>) , points to <root>
  Abstraction (1 occurence)     : Gamma(Variable, <body>), points to <root>
  Abstraction (2+ occurences)   : Gamma(Alpha, <body>)   , points to <root>
    Alphas then form a tree branching to all variables

  Application: Lambda(<root>, <function>), points to <argument>
  Variable: no extra nodes, points back to the Gammas / Alphas

  parse_var(): returns
    a) port to Alpha  = node whose left side is the variable
    b) port to Gamma  = head of an abstraction
    c) port to Lambda = node whose left side is root
  parse_head(): returns port to Gamma = head to abstraction
  parse_body(): returns
    a) port to Lambda = node whose left side is root
    b) port to Gamma  = head of an abstraction
    c) port to Alpha  = node whose left side is variable
*/

Port parse_term(const std::string &term, const term_map &defs, Net &net) {
    iss is(term);
    port_map vars = {};
    return parse_body(is, vars, defs, net);

}
Port parse_var(iss &is, port_map &vars, const term_map &defs, Net &net) {
    char c;
    is >> c;

    auto it = vars.find(c);
    if (it != vars.end()) {
        size_t gamma_idx = it->second;
        Node &gamma = net[gamma_idx];

        if (net[gamma.left].symbol == Epsilon) {
            net.pop(gamma.left.value);
            return {Left, gamma_idx};
        }

        Port current = gamma.left;
        size_t alpha = net.push({}, {}, {}, Alpha);

        net.link({Left, gamma_idx}, {Main, alpha});
        net.link(current, {Right, alpha});
        return {Left, alpha};
    }
    std::string s(1, c);
    while ((c = is.get()) != EOF and strchr(reserved, c) == nullptr)
        s += c;
    is.unget();

    auto it2 = defs.find(s);
    if (it2 != defs.end()) {
        std::string term = it2->second;
        iss is2(term);
        port_map local_context = {};
        return parse_body(is2, local_context, defs, net);
    }
    std::cerr << "ERROR not defined: " << s << '\n';
    return {None, 0};
}
Port parse_body(iss &is, port_map &vars, const term_map &defs, Net &net) {
    char c;
    Port next;
    Port previous = {Main, net.push({None, 0},{},{}, Root)};

    if (is.eof())
        return {};

    while ((is >> c) and c != ')') {
        if (c == '\\' or c == '@') {
            port_map local_context = {};
            next = parse_head(is, local_context, defs, net);
            is.putback(')');
        }
        else if (c == '(') {
            next = parse_body(is, vars, defs, net);
        }
        else {
            is.unget();
            next = parse_var(is, vars, defs, net);
        }

        if (net[previous].main.type == None) {
            net.link(previous, next);
            continue;
        }
        size_t lambda = net.push({}, {}, {}, Lambda);

        net.link({Right, lambda}, net[previous].main);
        net.link({Main, lambda}, next);

        net.link(previous, {Left, lambda});
    }
    Port root = net[previous].main;
    net.pop(previous);
    return root;
}
Port parse_head(iss &is, port_map &vars, const term_map &defs, Net &net) {
    char c;
    size_t first = (size_t)-1;
    size_t previous;

    while ((is >> c) and c != '.') {
        size_t gamma = net.push({}, {}, {}, Gamma);
        size_t epsilon = net.push({}, {None, 0}, {None, 0}, Epsilon);

        vars[c] = gamma;

        net.link({Left, gamma}, {Main, epsilon});

        if (first == (size_t)-1)
            first = gamma;
        else
            net.link({Right, previous},{Main, gamma});
        previous = gamma;
    }

    Port body = parse_body(is, vars, defs, net);

    net.link({Right, previous}, body);
    return {Main, first};
}
