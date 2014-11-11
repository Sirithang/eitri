#include "eitri.h"
#include <string.h>

eitri_OpsDatabase eitri_gOpsDB;


void eitri_init()
{
    eitri_registerOperations();
}


void eitri_registerOperations()
{
    //operation bitmap
    {
        strncpy(eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].name, "noise", 256);
        eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].inputImagesCount = 0;
        eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].func = eitri_noiseOp;

        eitri_gOpsDB.opsCount += 1;
    }
}


void eitri_createGraph(eitri_Graph *g)
{
    g->outputCount = 0;
    g->freeopsListCount = 0;
    g->outputFreeCount = 0;
    g->operationsCount = 0;
}

int eitri_addOutput(eitri_Graph *g)
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

void eitri_getOutput(eitri_Graph *g, const char *outputName)
{
    eitri_Output* out = 0;

    for(int i = 0; i < g->outputCount; ++i)
    {
        if(strncmp(outputName, g->outputs[i].name, 1024))
        {
            out = &g->outputs[i];
        }
    }

    //--------------


}

//------------------------------------------------

int eitri_addOperation(eitri_Graph *g, const char *name)
{
    int opIdx = -1;

    for(int i = 0; i < eitri_gOpsDB.opsCount; ++i)
    {
        if(strcmp(name, eitri_gOpsDB.ops[i].name) == 0)
        {
            opIdx = i;
            break;
        }
    }

    if(opIdx == -1)
        return opIdx;

    int idx;

    if(g->freeopsListCount > 0)
    {
        g->freeopsListCount -= 1;
        idx = g->freeops[g->freeopsListCount];
    }
    else
    {
        idx = g->operationsCount;
        g->operationsCount += 1;
    }

    g->operations[idx].operation = opIdx;
    //init all input to -1 (no input connected)
    memset(g->operations[idx].inputs, -1, 16 * sizeof(int));

    return opIdx;
}


//====================================================================

void eitri_noiseOp(eitri_OpFuncParams params)
{

}
