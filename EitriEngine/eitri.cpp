#include "eitri.h"
#include <string.h>

unsigned int opCount = 0;
Operation ops[1024];


void init()
{
    registerOperations();
}


void registerOperations()
{

}


void createGraph(Graph *g)
{
    g->outputCount = 0;
    g->freeopsListCount = 0;
    g->outputFreeCount = 0;
    g->operationsCount = 0;
}

int addOutput(Graph *g)
{
    int addIdx;

    if(g->outputFreeCount > 0)
    {
        addIdx = g->outputFree[g->outputFreeCount-1];
        g->outputFreeCount -= 1;
    }
    else
    {
        addIdx = g->outputCount;
        g->outputCount += 1;
    }

    strncat(g->outputs[addIdx].name, "New Output", 1024);

    return addIdx;
}

//-----------------------------------------------

void getOutput(Graph *g, const char *outputName)
{
    Output* out = 0;

    for(int i = 0; i < g->outputCount; ++i)
    {
        if(strncmp(outputName, g->outputs[i].name, 1024))
        {
            out = &g->outputs[i];
        }
    }

    //--------------


}
