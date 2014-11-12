#include "eitri.h"

#include <stdlib.h>
#include <string.h>

eitri_OpsDatabase eitri_gOpsDB;


void eitri_init()
{
    eitri_registerOperations();
}


void eitri_registerOperations()
{
    //operation output
    {
        strncpy(eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].name, "output", 256);
        eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].inputImagesCount = 1;
        eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].func = eitri_op_output;

        eitri_addParam(eitri_gOpsDB.opsCount, "test1", EITRI_PARAM_FLOAT);
        eitri_addParam(eitri_gOpsDB.opsCount, "test6", EITRI_PARAM_FLOAT);

        eitri_gOpsDB.opsCount += 1;
    }

    //operation bitmap
    {
        strncpy(eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].name, "noise", 256);
        eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].inputImagesCount = 0;
        eitri_gOpsDB.ops[eitri_gOpsDB.opsCount].func = eitri_op_noise;

        eitri_addParam(eitri_gOpsDB.opsCount, "blublu", EITRI_PARAM_FLOAT);

        eitri_gOpsDB.opsCount += 1;
    }
}


void eitri_createGraph(eitri_Graph *g)
{
    g->outputCount = 0;
    g->freeopsListCount = 0;
    g->outputFreeCount = 0;
    g->operationsCount = 0;

    g->seed = rand();
}

int eitri_getPictureDataSize(eitri_PicturData *d)
{
    return d->w * d->h * d->channelSize * d->nbChannel;
}

void eitri_allocatePictureData(eitri_PicturData* d)
{
    if(d->data)
    {
        free(d->data);
    }

    d->data = malloc(eitri_getPictureDataSize(d));
}

//-----------------------------------------------

void eitri_getOutput(eitri_Graph *g, const char *outputName)
{
//    eitri_Output* out = 0;

//    for(int i = 0; i < g->outputCount; ++i)
//    {
//        int outIdx = g->outputs[i];
//        if(outIdx == -1)
//            continue;//deleted output

//        if(strncmp(outputName, g->operations[outIdx].name, 1024))
//        {
//            out = &g->operations[i];
//        }
//    }

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

    eitri_OpInstance* inst = &g->operations[idx];
    inst->operation = opIdx;

    inst->isOutput = 0;

    //init all input to -1 (no input connected)
    memset(inst->inputs, -1, 16 * sizeof(int));

    inst->_cachedResult.w = 256;
    inst->_cachedResult.h = 256;
    inst->_cachedResult.nbChannel = 4;
    inst->_cachedResult.channelSize = sizeof(char);

    inst->_cachedResult.data = 0;

    if(strncmp(name, "output", 256) == 0)
    {//special case, output are cached for faster search
        int outIdx;

        if(g->outputFreeCount > 0)
        {
            g->outputFreeCount -= 1;
            outIdx = g->outputFree[g->outputFreeCount];
        }
        else
        {
            outIdx = g->outputCount;
            g->outputCount += 1;
        }

        g->outputs[outIdx] = idx;
        inst->isOutput = 1;
    }

    return idx;
}

//-----------------------------------------

void eitri_doOperation(eitri_Graph *g, int op)
{
    if(g->operations[op].operation != -1)
    {
        eitri_gOpsDB.ops[g->operations[op].operation].func(g, op);
    }
}

//------------------------------------------

void eitri_connectOps(eitri_Graph *g, int inputOps, int outputOps, int idx)
{
    eitri_OpInstance* in = &g->operations[inputOps];

    in->inputs[idx] = outputOps;
}

//----- param management

void eitri_addParam(int op, const char *name, eitri_ParamType type)
{
    int c = eitri_gOpsDB.ops[op].paramsCount;
    strncpy(eitri_gOpsDB.ops[op].params[c].name, name, 256);
    eitri_gOpsDB.ops[op].params[c].type = type;

    eitri_gOpsDB.ops[op].paramsCount += 1;
}


//====================================================================

void eitri_op_output(eitri_Graph* graph,  int opInst)
{
    eitri_OpInstance* inst = &graph->operations[opInst];

    if(inst->inputs[0] == -1)
        return;

    eitri_doOperation(graph, inst->inputs[0]);
    eitri_OpInstance* input1 = &graph->operations[inst->inputs[0]];

    if(input1->_cachedResult.data == 0)
        return;

    eitri_allocatePictureData(&inst->_cachedResult);

    //@TODO : change for dynamic sizing
    memcpy(inst->_cachedResult.data, input1->_cachedResult.data, eitri_getPictureDataSize(&input1->_cachedResult));
}

void eitri_op_noise(eitri_Graph* graph,  int opInst)
{
    eitri_OpInstance* inst = &graph->operations[opInst];

    eitri_allocatePictureData(&inst->_cachedResult);

    //this allow to be deterministic no matter when or how many this opInstance is called
    srand(graph->seed + opInst);

    for(int i = 0; i < inst->_cachedResult.w * inst->_cachedResult.h * 4; i += 4)
    {
        char val = rand() % 255;

        ((char*)inst->_cachedResult.data)[i+0] = val;
        ((char*)inst->_cachedResult.data)[i+1] = val;
        ((char*)inst->_cachedResult.data)[i+2] = val;
        ((char*)inst->_cachedResult.data)[i+3] = 255;
    }
}
