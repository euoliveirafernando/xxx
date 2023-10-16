#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "graph.h"
#include "opcode.h"
#include "gen.h"

static void opcode(node * xxx);

static void gen_character(char * character)
{
    byte("0x%x ", * character);
}

static void gen_constant(double * number)
{
    byte("%lf ", * number);
}

static void gen_variable(char * name)
{
    byte("$%s ", name);
}

static void gen_function(edge * xxx)
{
    byte("(");

    while (xxx != NULL) {
        opcode(xxx->v);
        
        if (xxx->v->succ != NULL) {
            xxx = xxx->v->succ;
            byte(", ");
        }
    }
    
    byte(")");
}

static void gen_op(char * name)
{
    byte("$%s := ", name);
}

static void opcode(node * xxx)
{
    if (xxx != NULL)
        switch (xxx->op) {
            case imp:
            case or:
            case and:
            case equal:
            case not_equal:
            case less:
            case greater:
            case less_equal:
            case greater_equal:
            case add:
            case sub:
            case mul:
            case div:
            case per:
            case pow:
            case rad:
            case log:
                gen_op(xxx->data);
                opcode(xxx->succ->succ_next->v);
                byte("%x", xxx->op);
                opcode(xxx->succ->v);
                byte("\n");
                break;
            case neg:
            case not:
            case det:
            case ln:
            case exp:
            case sqrt:
            case cbrt:
            case sin:
            case cos:
            case tan:
            case csc:
            case sec:
            case cot:
            case arcsin:
            case arccos:
            case arctan:
            case arccsc:
            case arcsec:
            case arccot:
                gen_op(xxx->data);
                byte("%x", xxx->op);
                opcode(xxx->succ->v);
                byte("\n");
                break;
            case ccharacter:
                gen_character(xxx->data);
                break;
            case constant:
                gen_constant(xxx->data);
                break;
            case variable:
                gen_variable(xxx->data);
                break;
            case function:
                gen_function(xxx->succ);
                break;
        }
}

static void code(node * xxx)
{
    if (xxx != NULL) {
        code(xxx->next);
        opcode(xxx);
    }
}

void gen(graph * g)
{
    if (g != NULL)
        code(g->next);
}