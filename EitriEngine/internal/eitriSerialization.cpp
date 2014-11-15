#include "eitri.h"
#include "eitriInternal.h"

#include <stdio.h>

void eitri_serializeGraph(eitri_Graph *g, char *dest, int maxSize)
{
    char* str = dest;

    str += sprintf(str, "{\n\t\"seed\": %i,\n", g->seed);
    str += sprintf(str, "\"nodes\": [ \n");

    for(int i = 0; i < g->operationsCount; ++i)
    {
        str = eitri_saveNode(&g->operations[i], str);

        if(i < g->operationsCount - 1)
            str += sprintf(str, ",\n");
    }

    str += sprintf(str, "]\n");

    str += sprintf(str, "}");
}


char* eitri_saveNode(eitri_OpInstance *inst, char *out)
{
    char* str = out;
    str += sprintf(str, "{\n");
    str += sprintf(str, "\t\"op\":%i,\n", inst->operation);
    str += sprintf(str, "\t\"isOutput\":%i,\n", inst->isOutput);

    str += sprintf(str, "\t\"inputs\":[");
    for(int i = 0; i < 16; ++i)
    {
        str += sprintf(str, "%i", inst->inputs[i]);
        if(i != 15)
            str += sprintf(str, ",");
    }

    str += sprintf(str, "]\n");

    str += sprintf(str, "}\n");

    return str;
}
