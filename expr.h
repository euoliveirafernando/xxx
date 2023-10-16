#ifndef _expr_h_
#define _expr_h_

#define lvalue(x)    x == variable || x == function

extern token kind;

extern bool next_token(token t);
extern void expect(u_char c);

extern node * create_node(u_long op);
extern void create_edge(node * u, node * v);
extern node * create_value(void);
extern node * uny(u_long op, node * lhs);
extern node * bin(node * lhs, u_long op, node * rhs);
extern node * tey(node * lhs, node * mid, node * rhs);

extern node * table_lookup(table * t);

extern void error(const char * format, ...);

#endif