#include "eitri.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>



#define FOR_PIXELS_IN_IMAGE(imageData) \
    for(int y = 0; y < imageData.h; ++y) \
    { \
        for(int x = 0; x < imageData.w; ++x) \
        { \

#define END_FOR_PIXELS \
        } \
    } \

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

        eitri_addParam(idx, "color", EITRI_PARAM_COLOR);

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

        eitri_addParam(idx, "seed", EITRI_PARAM_INT);

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
    //memcpy(inst->_cachedResult.data, input1->_cachedResult.data, eitri_getPictureDataSize(&input1->_cachedResult));

    FOR_PIXELS_IN_IMAGE(inst->_cachedResult)
            float u = x / (float)inst->_cachedResult.w;
            float v = y / (float)inst->_cachedResult.h;

            char* dest = eitri_samplePictureData(&inst->_cachedResult, u, v);
            char* src = eitri_samplePictureData(&input1->_cachedResult, u, v);

            *((eitri_Color*)dest) = *((eitri_Color*)src);
     END_FOR_PIXELS
}

void eitri_op_noise(eitri_Graph* graph,  int opInst)
{
    eitri_OpInstance* inst = &graph->operations[opInst];

    eitri_allocatePictureData(&inst->_cachedResult);

    //this allow to be deterministic no matter when or how many this opInstance is called
    srand(graph->seed + opInst);

    for(int i = 0; i < inst->_cachedResult.w * inst->_cachedResult.h; ++i)
    {
        char val = rand() % 255;

        eitri_Color* c = (eitri_Color*)(inst->_cachedResult.data) + i;

        c->r = val;
        c->g = val;
        c->b = val;
        c->a = 255;
    }
}

void eitri_op_color(eitri_Graph *graph, int opInst)
{
    eitri_OpInstance* inst = &graph->operations[opInst];

    eitri_allocatePictureData(&inst->_cachedResult);

    eitri_Color c = inst->paramsValues[0].colParam;

    *((eitri_Color*)inst->_cachedResult.data) = c;
}


void eitri_op_multiply(eitri_Graph *graph, int opInst)
{
    eitri_OpInstance* inst = &graph->operations[opInst];

    if(inst->inputs[0] == -1 || inst->inputs[1] == -1)
        return;

    eitri_doOperation(graph, inst->inputs[0]);
    eitri_doOperation(graph, inst->inputs[1]);

    eitri_OpInstance* input1 = &graph->operations[inst->inputs[0]];
    eitri_OpInstance* input2 = &graph->operations[inst->inputs[1]];

     eitri_allocatePictureData(&inst->_cachedResult);

     FOR_PIXELS_IN_IMAGE(inst->_cachedResult)
             float u = x / (float)inst->_cachedResult.w;
             float v = y / (float)inst->_cachedResult.h;

             char* dest = eitri_samplePictureData(&inst->_cachedResult, u, v);
             char* src1 = eitri_samplePictureData(&input1->_cachedResult, u, v);
             char* src2 = eitri_samplePictureData(&input2->_cachedResult, u, v);

             ((eitri_Color*)dest)->r = (((eitri_Color*)src1)->r * ((eitri_Color*)src2)->r) / 255;
             ((eitri_Color*)dest)->g = (((eitri_Color*)src1)->g * ((eitri_Color*)src2)->g) / 255;
             ((eitri_Color*)dest)->b = (((eitri_Color*)src1)->b * ((eitri_Color*)src2)->b) / 255;
             ((eitri_Color*)dest)->a = (((eitri_Color*)src1)->a * ((eitri_Color*)src2)->a) / 255;
     END_FOR_PIXELS
}


void eitri_op_brick(eitri_Graph *graph, int opInst)
{
    eitri_OpInstance* inst = &graph->operations[opInst];

    eitri_allocatePictureData(&inst->_cachedResult);

    int localSeed = inst->paramsValues[0].iParam;

    srand(graph->seed + opInst + localSeed);

    //---Get param value
    int brickHeight = 15;

    int nbLine = inst->_cachedResult.h / brickHeight;

    const int maxBrickPerLines = 10;

    //number of brick per line
    int* bricksNumbers = (int*)malloc(nbLine*sizeof(int));
    //pos of each
    int* brickPos = (int*)malloc(nbLine*maxBrickPerLines*sizeof(int));
    eitri_Color* brickCol = (eitri_Color*)malloc(nbLine*maxBrickPerLines*sizeof(eitri_Color));

    for(int i = 0; i < nbLine; ++i)
    {
        bricksNumbers[i] = 5 + (rand() % 4);

        for(int j = 0; j < bricksNumbers[i]; ++j)
        {
            int bidx = i * maxBrickPerLines + j;
            int median = inst->_cachedResult.w / bricksNumbers[i] * (j+1);
            brickPos[bidx] = median + ( 5 - (rand() % 10));

            int r = rand()%255;
            brickCol[bidx].a = 255;
            brickCol[bidx].r = r;
            brickCol[bidx].g = r;
            brickCol[bidx].b = r;
        }
    }

     FOR_PIXELS_IN_IMAGE(inst->_cachedResult)
            int line = y / brickHeight;
            int isDark = ((y%brickHeight) == 0);

            int brickIdx = -1;

            for(int i = 0; i < bricksNumbers[line]; ++i)
            {
                int val = brickPos[line * maxBrickPerLines + i];

                if(val > x && brickIdx == -1)
                {
                    brickIdx = i;
                }

                if(val == x)
                {
                    isDark |= 1;
                    break;
                }
            }

            eitri_Color* c = (eitri_Color*)(inst->_cachedResult.data) + (y * inst->_cachedResult.w + x);

            if(isDark)
            {//top/bottom of brick just put black

                c->a = 255;
                c->r = 0;
                c->g = 0;
                c->b = 0;
            }
            else
            {
               *c = brickCol[line * maxBrickPerLines + brickIdx];
            }
    END_FOR_PIXELS

    free(bricksNumbers);
    free(brickPos);
    free(brickCol);
}
