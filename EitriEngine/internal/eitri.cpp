#include "eitri.h"
#include "eitriInternal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


eitri_OpsDatabase eitri_gOpsDB;


void eitri_init()
{
    eitri_registerOperations();
}


void eitri_registerOperations()
{
    //operation output
    {
        int idx = eitri_gOpsDB.opsCount;

        strncpy(eitri_gOpsDB.ops[idx].name, "output", 256);
        eitri_gOpsDB.ops[idx].inputImagesCount = 1;
        eitri_gOpsDB.ops[idx].func = eitri_op_output;
        eitri_gOpsDB.ops[idx].size = -1;

        eitri_gOpsDB.opsCount += 1;
    }

    //operation bitmap
    {
        int idx = eitri_gOpsDB.opsCount;

        strncpy(eitri_gOpsDB.ops[idx].name, "noise", 256);
        eitri_gOpsDB.ops[idx].inputImagesCount = 0;
        eitri_gOpsDB.ops[idx].func = eitri_op_noise;
        eitri_gOpsDB.ops[idx].size = -1;

        eitri_gOpsDB.opsCount += 1;
    }

    //operation color
    {
        int idx = eitri_gOpsDB.opsCount;

        strncpy(eitri_gOpsDB.ops[idx].name, "color", 256);
        eitri_gOpsDB.ops[idx].inputImagesCount = 0;
        eitri_gOpsDB.ops[idx].func = eitri_op_color;
        eitri_gOpsDB.ops[idx].size = 1;

        eitri_addParam(idx, "color", "", EITRI_PARAM_COLOR);

        eitri_gOpsDB.opsCount += 1;
    }

    //operation multiply
    {
        int idx = eitri_gOpsDB.opsCount;

        strncpy(eitri_gOpsDB.ops[idx].name, "multiply", 256);
        eitri_gOpsDB.ops[idx].inputImagesCount = 2;
        eitri_gOpsDB.ops[idx].func = eitri_op_multiply;
        eitri_gOpsDB.ops[idx].size = -1;

        eitri_gOpsDB.opsCount += 1;
    }

    //operation brick
    {
        int idx = eitri_gOpsDB.opsCount;

        strncpy(eitri_gOpsDB.ops[idx].name, "bricks", 256);
        eitri_gOpsDB.ops[idx].inputImagesCount = 0;
        eitri_gOpsDB.ops[idx].func = eitri_op_brick;
        eitri_gOpsDB.ops[idx].size = -1;

        eitri_addParam(idx, "seed", "random number seed", EITRI_PARAM_INT);

        eitri_gOpsDB.opsCount += 1;
    }

    //operation perlin noise
    {
        int idx = eitri_gOpsDB.opsCount;

        strncpy(eitri_gOpsDB.ops[idx].name, "perlin", 256);
        eitri_gOpsDB.ops[idx].inputImagesCount = 0;
        eitri_gOpsDB.ops[idx].func = eitri_op_perlin;
        eitri_gOpsDB.ops[idx].size = -1;

        eitri_addParam(idx, "xMul", "x multiplier for perlin noise", EITRI_PARAM_FLOAT);
        eitri_addParam(idx, "yMul", "y multiplier for perlin noise", EITRI_PARAM_FLOAT);
        eitri_addParam(idx, "zMul", "z multiplier for perlin noise", EITRI_PARAM_FLOAT);

        eitri_addParam(idx, "wrap", "if 1 and x&y multiplier is power of 2, the noise wrap", EITRI_PARAM_INT);

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


//----------- PictureData

void eitri_allocatePictureData(eitri_PicturData* d)
{
    if(d->data)
    {
        free(d->data);
    }

    d->data = (char*)malloc(eitri_getPictureDataSize(d));
}

char* eitri_samplePictureData(eitri_PicturData *d, float u, float v)
{
    //default is wrapping, maybe offer choice?
    float cu = u - floor(u);
    float cv = v - floor(v);

    int x = cu * d->w;
    int y = cv * d->h;

    return (d->data + (y * d->w * d->nbChannel + x * d->nbChannel));
}

//---------------- Params

eitri_OpParamValue eitri_getDefaultParamValue(eitri_ParamType type)
{
    eitri_OpParamValue val;

    switch(type)
    {
    case EITRI_PARAM_FLOAT:
        val.fParam = 0.0f;
        break;
    case EITRI_PARAM_STRING:
        val.sParam[0] = 0; // empty string
        break;
    case EITRI_PARAM_COLOR:
        val.colParam.r = 0;
        val.colParam.g = 0;
        val.colParam.b = 0;
        val.colParam.a = 255;
        break;
    default:
        break;
    }

    return val;
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

void eitri_initOp(eitri_Graph* g, int idx, int opIdx)
{
    eitri_OpInstance* inst = &g->operations[idx];
    inst->operation = opIdx;

    inst->isOutput = 0;

    for(int i = 0; i < eitri_gOpsDB.ops[opIdx].paramsCount; ++i)
    {
        inst->paramsValues[i] = eitri_getDefaultParamValue(eitri_gOpsDB.ops[opIdx].params[i].type);
    }

    //init all input to -1 (no input connected)
    memset(inst->inputs, -1, 16 * sizeof(int));

    inst->_cachedResult.w = eitri_gOpsDB.ops[opIdx].size == -1 ? 256 : eitri_gOpsDB.ops[opIdx].size;
    inst->_cachedResult.h = eitri_gOpsDB.ops[opIdx].size == -1 ? 256 : eitri_gOpsDB.ops[opIdx].size;
    inst->_cachedResult.nbChannel = 4;
    inst->_cachedResult.channelSize = sizeof(char);

    inst->_cachedResult.data = 0;
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

    eitri_initOp(g,idx,opIdx);

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
        g->operations[idx].isOutput = 1;
    }

    return idx;
}


void eitri_deleteOperation(eitri_Graph *g, int op)
{
    eitri_OpInstance* inst = &g->operations[op];

    for(int i = 0; i < eitri_gOpsDB.ops[inst->operation].inputImagesCount; ++i)
    {
        eitri_disconnectOps(g, op, i);
    }

    inst->operation = -1;
    if(inst->_cachedResult.data)
        free(inst->_cachedResult.data);

    inst->_cachedResult.w = 0;
    inst->_cachedResult.h = 0;

    g->freeops[g->freeopsListCount] = op;
    g->freeopsListCount += 1;
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

void eitri_disconnectOps(eitri_Graph *g, int ops, int idx)
{
    g->operations[ops].inputs[idx] = -1;
}

//----- param management

void eitri_addParam(int op, const char *name, const char *tip, eitri_ParamType type)
{
    int c = eitri_gOpsDB.ops[op].paramsCount;
    strncpy(eitri_gOpsDB.ops[op].params[c].name, name, 256);
    strncpy(eitri_gOpsDB.ops[op].params[c].tip, tip, 256);
    eitri_gOpsDB.ops[op].params[c].type = type;

    eitri_gOpsDB.ops[op].paramsCount += 1;
}


//====================================================================


