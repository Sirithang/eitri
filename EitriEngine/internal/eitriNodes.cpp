#include "eitri.h"
#include "eitriInternal.h"

#include <stdlib.h>
#include <math.h>

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

#define FOR_PIXELS_IN_IMAGE(imageData) \
    for(int y = 0; y < imageData.h; ++y) \
    { \
        for(int x = 0; x < imageData.w; ++x) \
        { \
            float u = x / (float)imageData.w; \
            float v = y / (float)imageData.h; \

#define END_FOR_PIXELS \
        } \
    } \




void eitri_op_output(eitri_Graph* graph,  int opInst)
{
    eitri_NodeInstance* inst = &graph->operations[opInst];

    if(inst->inputs[0] == -1)
        return;

    eitri_doOperation(graph, inst->inputs[0]);
    eitri_NodeInstance* input1 = &graph->operations[inst->inputs[0]];

    if(input1->_cachedResult.data == 0)
        return;

    eitri_allocatePictureData(&inst->_cachedResult);

    FOR_PIXELS_IN_IMAGE(inst->_cachedResult)

            char* dest = eitri_samplePictureData(&inst->_cachedResult, u, v);
            char* src = eitri_samplePictureData(&input1->_cachedResult, u, v);

            *((eitri_Color*)dest) = *((eitri_Color*)src);
     END_FOR_PIXELS
}

void eitri_op_noise(eitri_Graph* graph,  int opInst)
{
    eitri_NodeInstance* inst = &graph->operations[opInst];

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
    eitri_NodeInstance* inst = &graph->operations[opInst];

    eitri_allocatePictureData(&inst->_cachedResult);

    eitri_Color c = inst->paramsValues[0].colParam;

    *((eitri_Color*)inst->_cachedResult.data) = c;
}


void eitri_op_multiply(eitri_Graph *graph, int opInst)
{
    eitri_NodeInstance* inst = &graph->operations[opInst];

    if(inst->inputs[0] == -1 || inst->inputs[1] == -1)
        return;

    eitri_doOperation(graph, inst->inputs[0]);
    eitri_doOperation(graph, inst->inputs[1]);

    eitri_NodeInstance* input1 = &graph->operations[inst->inputs[0]];
    eitri_NodeInstance* input2 = &graph->operations[inst->inputs[1]];

     eitri_allocatePictureData(&inst->_cachedResult);

     FOR_PIXELS_IN_IMAGE(inst->_cachedResult)

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
    eitri_NodeInstance* inst = &graph->operations[opInst];

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

void eitri_op_perlin(eitri_Graph* graph, int opInst)
{
    eitri_NodeInstance* inst = &graph->operations[opInst];

    eitri_allocatePictureData(&inst->_cachedResult);

    float xMul = inst->paramsValues[0].fParam;
    float yMul = inst->paramsValues[1].fParam;
    float zMul = inst->paramsValues[2].fParam;

    FOR_PIXELS_IN_IMAGE(inst->_cachedResult)
        float f = stb_perlin_noise3(u * xMul, v * yMul, 1.0f * zMul, xMul, yMul, 0);

        eitri_Color* dest = (eitri_Color*)eitri_samplePictureData(&inst->_cachedResult, u, v);

        dest->a = 255;
        dest->r = f * 255;
        dest->g = f * 255;
        dest->b = f * 255;

    END_FOR_PIXELS
}
