# VisualNets
A visual interpreter of a system of Interaction Nets directly emulating the Lambda Calculus

---

## What are Interaction Nets

Interaction Nets are a theoretical model of computation by Yves Lafont.
A net consists of a finite number of 'agents' connected by edges much like nodes in a graph, however unlike nodes, agents number their connected edges into ports.

The first port is called the 'principal port', or the zero-th port, while the remaining ports are called auxiliary and are numbered 1 to n-1.

Every agent is characterized by a single 'symbol', which determines its arity and rules which are applied to it.

Whenever an edge connects the principal ports of two agents, an interaction rule can be executed, based on the symbols of the two involved agents. these rules have to be symmetric, meaning with n symbols there may be up to n(n-1)/2 unique interaction rules, tho not all may be defined.
The two agents are first erased from the net, then some number of new agents is added, and edges are connected between them and the rest of the net.

## What is this project

Though simple this computing model is turing complete. The goal of this project is to provide a simple environment for testing and simulating large nets and rule systems, as well as showcase a system of rules involving 5 symbols which can directly represent terms of the Lambda Calculus and their beta reduction.

