#pragma once

#include "net.h"
#include <variant>
#include <map>

using RuleList = std::vector<std::variant<symbolid_t, portid_t>>;

struct Reduction {
    RuleList left;
    RuleList right;
};
using SymbolPair = std::pair<symbolid_t, symbolid_t>;

struct Reducer {
    void reconnect_ports(nodeid_t id, nodeid_t refid, const RuleList &rulelist, std::map<portid_t, Port> &lookup_table, Net &net) const;
    std::map<SymbolPair, Reduction> reductions;
public:
    void register_rule(const SymbolPair &symbol_pair, const Reduction &reduction);
    void reduce(nodeid_t leftid, nodeid_t rightid, Net &net) const;
    void reduce_all(float distance, Net &net) const;
};
