#include <string.h>
#include "type.h"
#include "graph.h"
#include "opcode.h"
#include "table.h"
#include "hash.h"

#define max 2048

table * create_table(table * next)
{
    table * t;

    t = cmath_alloc(sizeof (table));

    t->hash = cmath_alloc(sizeof (list * [max]));

    for (int i = 0; i < max; i++)
        t->hash[i] = create_list();

    t->next = next;

    return t;
}

static u_long hash(char * name)
{
    size_t length;
    u_long value = 0;

    length = strlen(name);

    for (int i = 0; i < length; i++) {
        value <<= 8;
        value += name[i];
    }

    return value % max;
}

void table_insert(table * t, node * xxx)
{
    u_long value;
    
    value = hash(xxx->data);
    list_insert(t->hash[value], xxx);
}

node * lookup(table * t, u_long value)
{
    node * xxx;

    if (t == NULL)
        return NULL;
    else {
        xxx = list_lookup(t->hash[value]);

        if (xxx == NULL)
            return lookup(t->next, value);
        else
            return xxx;
    }
}

node * table_lookup(table * t)
{
    u_long value;
    node * xxx;
    
    value = hash(identifier);
    xxx = lookup(t, value);
    
    if (xxx == NULL) {
        xxx = create_node(variable);
        xxx->data = identifier;
    }
    
    get_next_token();
    
    return xxx;
}