#pragma once

extern "C"
{
    typedef struct
    {
        unsigned int w,h;
        unsigned int nbChannel;
        unsigned int channelSize;
        void* data;
    } eitri_PicturData;

    int eitri_getPictureDataSize(eitri_PicturData* d);
    //allocate (& free previously existing) buffer
    void eitri_allocatePictureData(eitri_PicturData* d);

    //=========================================================

    typedef enum
    {
        EITRI_PARAM_FLOAT,
        EITRI_PARAM_STRING
    }eitri_ParamType;

    typedef union
    {
        float fParam;
        char sParam[2048];
    } eitri_OpParamValue;

    typedef struct
    {
        eitri_ParamType type;
        char name[1024];
    } eitri_OpParams;

    //=========================================================

    typedef struct
    {
        int operation;
        int inputs[16];

        //this got a 1:1 mapping to params in eitri_Operations
        eitri_OpParamValue paramsValues[32];

        eitri_PicturData _cachedResult;
        int isOutput;
    } eitri_OpInstance;

    //==========================================================

    typedef struct
    {
        unsigned char       outputCount;
        int        outputs[256];

        unsigned char       outputFreeCount;
        unsigned char       outputFree[256];

        unsigned int        operationsCount;
        eitri_OpInstance    operations[2048];

        unsigned int        freeopsListCount;
        unsigned int        freeops[1024];


        unsigned int        seed;
    } eitri_Graph;


    //===================================================

    //inst is the instance of the operation. It contain all info for
    //doing the op (connected input, where to output etc...
    typedef void (*eitri_opFunc)(eitri_Graph* graph, int opInst);

    typedef struct
    {
        //
        unsigned char inputImagesCount;

        //this tell how many channel is inputed from each input
        // -1 (256) mean dynamic input (use the number of the output plug there)
        unsigned char inputChanelCount[256];

        //this function is called to perform the operation
        eitri_opFunc func;

        eitri_OpParams params[32];
        unsigned int paramsCount;

        char name[256];
    } eitri_Operation;

    //===========================================================

    typedef struct
    {
        int         opsCount;
        eitri_Operation   ops[1024];
    } eitri_OpsDatabase;

    extern eitri_OpsDatabase eitri_gOpsDB;

    void eitri_init();

    //this is called by the engine on init.
    //it create the map op name <-> associated Op
    //add new ops in that functions
    void eitri_registerOperations();

    void eitri_createGraph(eitri_Graph* g);

    int eitri_addOperation(eitri_Graph* g, const char* name);

    //execute the operation, calling recursivly execute on input
    //and stocking result in its _cachedData
    void eitri_doOperation(eitri_Graph* g, int op);

    void eitri_connectOps(eitri_Graph* g, int inputOps, int outputOps, int idx);

    int eitri_generateOutput(eitri_Graph* g, const char* outputName);

    // --- param management

    //op here is the op template, not the op instance. This is rarely called by user
    //only in eitri_registerOperations to define all info
    void eitri_addParam(int op, const char* name, eitri_ParamType type);

    //============== operation impl. =============================

    void eitri_op_output(eitri_Graph* graph, int opInst);
    void eitri_op_noise(eitri_Graph* graph,  int opInst);
}
