#pragma once

extern "C"
{

    typedef struct
    {
        unsigned char b,g,r,a;
    } eitri_Color;


    typedef struct
    {
        unsigned int w,h;
        unsigned int nbChannel;
        unsigned int channelSize;
        char* data;
    } eitri_PicturData;

    int eitri_getPictureDataSize(eitri_PicturData* d);
    //allocate (& free previously existing) buffer
    void eitri_allocatePictureData(eitri_PicturData* d);

    //this return the adress corresponding to the u/v adress (only decimal part is used)
    char* eitri_samplePictureData(eitri_PicturData* d, float u, float v);

    //=========================================================

    typedef enum
    {
        EITRI_PARAM_FLOAT,
        EITRI_PARAM_INT,
        EITRI_PARAM_STRING,
        EITRI_PARAM_COLOR
    }eitri_ParamType;

    typedef union
    {
        float fParam;
        int iParam;
        eitri_Color colParam;
        char sParam[2048];
    } eitri_OpParamValue;

    typedef struct
    {
        eitri_ParamType type;
        char name[1024];
    } eitri_OpParams;

    //return the default value for given type ( for float, (0,0,0,1) for color )
    eitri_OpParamValue eitri_getDefaultParamValue(eitri_ParamType type);

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
        //how many input that op take
        unsigned char inputImagesCount;

        //this function is called to perform the operation
        eitri_opFunc func;

        eitri_OpParams params[32];
        unsigned int paramsCount;

        //if this is -1, size is dynamic. If set, node only operate in that size*size
        //ATM this is only used by the Color node, @TODO: maybe find a beter way?
        int size;

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
    void eitri_deleteOperation(eitri_Graph* g, int op);

    //execute the operation, calling recursivly execute on input
    //and stocking result in its _cachedData
    void eitri_doOperation(eitri_Graph* g, int op);

    void eitri_connectOps(eitri_Graph* g, int inputOps, int outputOps, int idx);
    void eitri_disconnectOps(eitri_Graph* g, int ops, int idx);

    int eitri_generateOutput(eitri_Graph* g, const char* outputName);

    // --- param management

    //op here is the op template, not the op instance. This is rarely called by user
    //only in eitri_registerOperations to define all info
    void eitri_addParam(int op, const char* name, eitri_ParamType type);

    //============== operation impl. =============================

    void eitri_op_output(eitri_Graph* graph, int opInst);
    void eitri_op_noise(eitri_Graph* graph,  int opInst);
    void eitri_op_color(eitri_Graph* graph, int opInst);
    void eitri_op_multiply(eitri_Graph* graph, int opInst);

    void eitri_op_brick(eitri_Graph* graph, int opInst);
}
