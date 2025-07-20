#include "lambda_net.h"
#include <iomanip>

void VOID(size_t left, size_t right, Net &net) {
    net.pop(left);
    net.pop(right);
}

void ERAS(size_t pleft, size_t pright, Net &net) {
    Node &right = net[pright];

    right.symbol = Epsilon;

    net.link(right.right, {Main, pleft});
    net.link(right.left, {Main, pright});

    right.right = {None, 0};
    right.left = {None, 0};
}

void ANNI(size_t pleft, size_t pright, Net &net) {
    Node &left = net[pleft];
    Node &right = net[pright];

    net.link(right.right, left.right);
    net.link(right.left, left.left);

    net.pop(pleft);
    net.pop(pright);
}

void ULCK(size_t pleft, size_t pright, Net &net) {
    Node &right = net[pright];

    right.symbol = Gamma;

    net.link(right.right, {Main, pright});
    net.link(right.left, {Right, pright});
    net.link({Main, pleft}, {Left, pright});
}

void PROP(size_t pleft, size_t pright, Net &net) {
    Node &left = net[pleft];
    Node &right = net[pright];

    right.symbol = Delta;

    size_t leftcpy = net.push(left);
    size_t rightcpy = net.push(right);

    net.link(left.left, {Main, pright});
    net.link(right.right, {Main, pleft});

    net.link(left.right, {Main, rightcpy});
    net.link(right.left, {Main, leftcpy});

    net.link({Left, pleft}, {Right, pright});
    net.link({Left, leftcpy}, {Left, pright});

    net.link({Right, pleft}, {Right, rightcpy});
    net.link({Right, leftcpy}, {Left, rightcpy});
}

constexpr Rule RuleTable[25] = {
    ANNI, NULL, NULL, NULL, NULL,
    PROP, NULL, NULL, NULL, NULL,
    PROP, NULL, NULL, NULL, NULL,
    PROP, PROP, ULCK, ANNI, NULL,
    ERAS, ERAS, ERAS, ERAS, VOID
};
void Net::interact(size_t left, size_t right) {
    if (nodes[left].symbol > nodes[right].symbol)
        RuleTable[nodes[left].symbol * 5 + nodes[right].symbol](left, right, *this);
    else
        RuleTable[nodes[right].symbol * 5 + nodes[left].symbol](right, left, *this);
}
void Net::link(const Port &left, const Port &right) {
    (*this)[right][right.type] = left;
    (*this)[left][left.type] = right;
}
static constexpr const char * TypeNames[6] = {
    "Delta", "Alpha", "Lamda", "Gamma", "Epsln", "Root "
};
static constexpr const char * PortNames[3] = {
    "Main  ", "Left  ", "Right "
};

inline void print_port(const Port& port, std::ostream &os) {
    if (port.type == None)
        os << " |           ";
    else
        os << " | " << PortNames[port.type] << std::setw(4) << port.value;
}

std::ostream& operator<<(std::ostream &os, const Net &net) {
    os << "     NODES |       MAIN |       LEFT |      RIGHT\n";
    os << "-------------------------------------------------\n";

    for (size_t i = 0; i < net.nodes.capacity(); ++i) {
        if (net.nodes[i].is_index())
            continue;
        const Node &node = net.nodes[i];

        os << std::setw(4) << i << " " << TypeNames[node.symbol];
        print_port(node.main, os);
        print_port(node.left, os);
        print_port(node.right, os);
        os << "\n";
    }
    return os;
}
