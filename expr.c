#include <stdlib.h>
#include <stdbool.h>
#include "type.h"
#include "token.h"
#include "graph.h"
#include "opcode.h"
#include "table.h"
#include "expr.h"

static node * assign(node * xxx, table * t);

static void expect_lvalue(u_long op)
{
    if (!(lvalue(op)))
        error("lvalue expected");
}

static node * call(node * xxx, table * t)
{
    node * lhs;
    
    if (next_token(')'))
        return NULL;
    
    do lhs = assign(xxx, t);
    while (next_token(','));
    
    expect(')');
    
    return lhs;
}

static node * bracket(node * xxx, table * t)
{
    node * lhs;
    
    if (next_token(']'))
        return NULL;
    
    lhs = assign(xxx, t);
    
    expect(']');
    
    return lhs;
}

static node * postfix(node * xxx, table * t)
{
    node * lhs;
    node * rhs;
    u_long op;
    
    lhs = table_lookup(t);
    while (post_expr(kind)) {
        if (next_token('(')) {
            op = function;
            rhs = call(xxx, t);
        } else if (next_token('[')) {
            op = subscript;
            rhs = bracket(xxx, t);
        } else if (next_token('.')) {
            op = access;
            rhs = table_lookup(t);
        }
        expect_lvalue(lhs->op);
        lhs = bin(lhs, op, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * primary(node * xxx, table * t)
{
    node * lhs;
    
    if (kind == token_ident)
        lhs = postfix(xxx, t);
    else if (next_token('(')) {
        lhs = assign(xxx, t);
        expect(')');
    } else if (next_token('|')) {
        lhs = assign(xxx, t);
        expect('|');
    } else if (primary_expr(kind))
        lhs = create_value();
    else
        error("expression expected");
    
    return lhs;
}

static node * matrix(node * xxx, table * t)
{
    node * lhs;
    
    if (next_token('[')) {
        do lhs = matrix(xxx, t);
        while (next_token(','));
        expect(']');
    } else
        lhs = primary(xxx, t);
    
    return lhs;
}

static node * tuple(node * xxx, table * t)
{
    node * lhs;
    
    if (next_token('{')) {
        do lhs = assign(xxx, t);
        while (next_token(','));
        expect('}');
    } else
        lhs = matrix(xxx, t);
    
    return lhs;
}

static node * unary(node * xxx, table * t)
{
    node * lhs;
    u_long op;
    
    if (next_token('+'))
        return unary(xxx, t);
    else if (next_token('-'))
        op = neg;
    else if (next_token('~'))
        op = not;
    else if (next_token(token_det))
        op = det;
    else if (next_token(token_ln))
        op = ln;
    else if (next_token(token_exp))
        op = exp;
    else if (next_token(token_sqrt))
        op = sqrt;
    else if (next_token(token_cbrt))
        op = cbrt;
    else if (next_token(token_sin))
        op = sin;
    else if (next_token(token_cos))
        op = cos;
    else if (next_token(token_tan))
        op = tan;
    else if (next_token(token_csc))
        op = csc;
    else if (next_token(token_sec))
        op = sec;
    else if (next_token(token_cot))
        op = cot;
    else if (next_token(token_arcsin))
        op = arcsin;
    else if (next_token(token_arccos))
        op = arccos;
    else if (next_token(token_arctan))
        op = arctan;
    else if (next_token(token_arccsc))
        op = arccsc;
    else if (next_token(token_arcsec))
        op = arcsec;
    else if (next_token(token_arccot))
        op = arccot;
    else
        return tuple(xxx, t);
    lhs = unary(xxx, t);
    lhs = uny(op, lhs);
    create_edge(xxx, lhs);
    
    return lhs;
}

static node * pown(node * xxx, table * t)
{
    node * lhs;
    node * rhs;
    u_long op;

    lhs = unary(xxx, t);
    while (pow_expr(kind)) {
        if (next_token('^'))
            op = pow;
        else if (next_token(token_rad))
            op = rad;
        else if (next_token(token_log))
            op = log;
        rhs = unary(xxx, t);
        lhs = bin(lhs, op, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * prod(node * xxx, table * t)
{
    node * lhs;
    node * rhs;
    u_long op;

    lhs = pown(xxx, t);
    while (prod_expr(kind)) {
        if (next_token('*'))
            op = mul;
        else if (next_token('/'))
            op = div;
        else if (next_token('%'))
            op = per;
        rhs = pown(xxx, t);
        lhs = bin(lhs, op, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * sum(node * xxx, table * t)
{
    node * lhs;
    node * rhs;
    u_long op;

    lhs = prod(xxx, t);
    while (sum_expr(kind)) {
        if (next_token('+'))
            op = add;
        else if (next_token('-'))
            op = sub;
        rhs = prod(xxx, t);
        lhs = bin(lhs, op, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * rel(node * xxx, table * t)
{
    node * lhs;
    node * rhs;
    u_long op;

    lhs = sum(xxx, t);
    while (rel_expr(kind)) {
        if (next_token('<'))
            op = less;
        else if (next_token('>'))
            op = greater;
        else if (next_token(token_less_equal))
            op = less_equal;
        else if (next_token(token_greater_equal))
            op = greater_equal;
        rhs = sum(xxx, t);
        lhs = bin(lhs, op, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * rel_eq(node * xxx, table * t)
{
    node * lhs;
    node * rhs;
    u_long op;

    lhs = rel(xxx, t);
    while (rel_eq_expr(kind)) {
        if (next_token(token_equal))
            op = equal;
        else if (next_token(token_not_equal))
            op = not_equal;
        rhs = rel(xxx, t);
        lhs = bin(lhs, op, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * con(node * xxx, table * t)
{
    node * lhs;
    node * rhs;

    lhs = rel_eq(xxx, t);
    while (next_token(token_and)) {
        rhs = rel_eq(xxx, t);
        lhs = bin(lhs, and, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * dis(node * xxx, table * t)
{
    node * lhs;
    node * rhs;

    lhs = con(xxx, t);
    while (next_token(token_or)) {
        rhs = con(xxx, t);
        lhs = bin(xxx, or, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * ifx(node * xxx, table * t)
{
    node * lhs;
    node * rhs;

    lhs = dis(xxx, t);
    while (next_token(token_con)) {
        rhs = dis(xxx, t);
        lhs = bin(xxx, imp, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * ify(node * xxx, table * t)
{
    node * lhs;
    node * mid;
    node * rhs;

    lhs = ifx(xxx, t);
    if (next_token('?')) {
        mid = assign(xxx, t);
        expect(':');
        rhs = ify(xxx, t);
        lhs = tey(lhs, mid, rhs);
        create_edge(xxx, lhs);
    }

    return lhs;
}

static node * assign(node * xxx, table * t)
{
    node * lhs;
    node * rhs;
    u_long op;

    lhs = ify(xxx, t);
    if (next_token('=')) {
        expect_lvalue(lhs->op);
        rhs = assign(xxx, t);
        lhs = bin(lhs, store, rhs);
        create_edge(lhs, rhs);
    }

    return lhs;
}

node * expr(node * xxx, table * t)
{
    node * lhs;
    
    lhs = assign(xxx, t);
    
    return lhs;
}