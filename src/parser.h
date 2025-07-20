#pragma once

#include "lambda_net.h"
#include "graphics.h"
#include <map>
#include <sstream>
#include <expected>
#include <cstring>

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

using term_map = std::map<std::string, std::string>;
using port_map = std::map<char, size_t>;
using iss      = std::istringstream;
inline const char *reserved = "()\\@. \n\t\r\0";

constexpr Vector2 step_right = {NODE_SIZE * 4, NODE_SIZE * 4};
constexpr Vector2 step_down = {NODE_SIZE * 4, 0};

Port parse_term(const std::string &term, const term_map &defs, Net &net);
Port parse_var(iss &is, port_map &vars, const term_map &defs, Net &net);
Port parse_body(iss &is, port_map &vars, const term_map &defs, Net &net);
Port parse_head(iss &is, port_map &vars, const term_map &defs, Net &net);
