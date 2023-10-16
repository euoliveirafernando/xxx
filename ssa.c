#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "token.h"
#include "graph.h"
#include "opcode.h"
#include "ssa.h"

graph * create_graph(const char * name)
{
    graph * g;

    input = fopen(name, "r");

    if (input == NULL)
        error("%s: no such file", name);

    g = cmath_alloc(sizeof (graph));

    return g;
}

node * create_node(u_long op)
{
    node * succ;

    succ = cmath_alloc(sizeof (node));
    succ->op = op;
    succ->addr = is_op(op) ? addr++ : 0;

    return succ;
}

void create_edge(node * u, node * v)
{
    edge * e;

    if (u == NULL || v == NULL)
        return;

    e = cmath_alloc(sizeof (edge));

    e->u = u;
    e->v = v;
    e->pred_next = v->pred;
    e->succ_next = u->succ;
    u->succ = v->pred = e;
}

node * create_value(void)
{
    node * succ;
    
    switch (kind) {
        case token_integer_literal:
        case token_floating_point:
        case token_character_constant:
        case token_string_literal:
            succ = create_node(constant);
            get_next_token();
            return succ;
        case token_euler:
            succ = create_node(euler);
            get_next_token();
            return succ;
        case token_complex:
            succ = create_node(complex);
            get_next_token();
            return succ;
        case token_pi:
            succ = create_node(pi);
            get_next_token();
            return succ;
    }
}

node * uny(u_long op, node * lhs)
{
    node * succ;

    succ = create_node(op);

    create_edge(succ, lhs);

    return succ;
}

node * bin(node * lhs, u_long op, node * rhs)
{
    node * succ;

    succ = create_node(op);
    
    create_edge(succ, lhs);
    create_edge(succ, rhs);

    return succ;
}

node * tey(node * lhs, node * mid, node * rhs)
{
    node * succ;

    succ = create_node(ter);
    
    create_edge(succ, lhs);
    create_edge(succ, mid);
    create_edge(succ, rhs);
}