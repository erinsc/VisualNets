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
        nodeid_t gamma_idx = it->second;
        Node &gamma = net.nodes[gamma_idx];
        Port current = gamma.ports[1];

        if (net.nodes[current.nodeid].symbol == Epsilon) {
            net.nodes.pop(current.nodeid);
            return {gamma_idx, 1};
        }
        nodeid_t alpha = net.add_node({0,0}, 0, Alpha);

        net.add_edge({gamma_idx, 1}, {alpha, 0});
        net.add_edge(current, {alpha, 2});
        return {alpha, 1};
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
    return {(nodeid_t)-1, (portid_t)-1};
}
Port parse_body(iss &is, port_map &vars, const term_map &defs, Net &net) {
    char c;
    Port next;
    nodeid_t false_nodeid = net.add_node({0,0}, 0, Alpha);
    Port false_port = {false_nodeid, 0};

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

        if (net.nodes[false_nodeid].ports[0].portid == (portid_t)-1) {
            net.add_edge(false_port, next);
            continue;
        }
        nodeid_t lambda = net.add_node({0,0}, 0, Lambda);

        net.add_edge({lambda, 2}, net.nodes[false_nodeid].ports[0]);
        net.add_edge({lambda, 0}, next);

        net.add_edge(false_port, {lambda, 1});
    }
    Port root = net.nodes[false_nodeid].ports[0];
    net.nodes.pop(false_nodeid);
    return root;
}
Port parse_head(iss &is, port_map &vars, const term_map &defs, Net &net) {
    char c;
    nodeid_t first = (nodeid_t)-1;
    nodeid_t previous = (nodeid_t)-1;

    while ((is >> c) and c != '.') {
        nodeid_t gamma = net.add_node({0,0}, 0, Gamma);
        nodeid_t epsilon = net.add_node({0,0}, 0, Epsilon);

        vars[c] = gamma;

        net.add_edge({gamma, 1}, {epsilon, 0});

        if (first == (nodeid_t)-1)
            first = gamma;
        else
            net.add_edge({previous, 2}, {gamma, 0});

        previous = gamma;
    }

    Port body = parse_body(is, vars, defs, net);

    net.add_edge({previous, 2}, body);
    return {first, 0};
}
