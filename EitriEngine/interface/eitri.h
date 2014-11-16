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
        char sParam[1024];
    } eitri_NodeParamValue;

    typedef struct
    {
        eitri_ParamType type;
        char name[256];
        char tip[512];
    } eitri_NodeParams;

    //return the default value for given type ( for float, (0,0,0,1) for color )
    eitri_NodeParamValue eitri_getDefaultParamValue(eitri_ParamType type);

    //=========================================================

    typedef struct
    {
        int operation;
        int inputs[16];

        //this got a 1:1 mapping to params in eitri_Operations
        eitri_NodeParamValue paramsValues[8];

        eitri_PicturData _cachedResult;
        int isOutput; //0 if not output, outputidx + 1 if output (hackish but allow to keep track without 2 param)

        //this flag is set by anything modifying the instance.
        //if it's == 0 then the image isn't recomputed, and the cached data is used
        int isDirty;
    } eitri_NodeInstance;

    //==========================================================

    typedef struct
    {
        char name[256];
        int node;
    } eitri_GraphOutput;

    typedef struct
    {
        unsigned char   outputCount;
        eitri_GraphOutput     outputs[16];

        unsigned char       outputFreeCount;
        unsigned char       outputFree[16];

        unsigned int        nodeCount;
        eitri_NodeInstance    nodes[1024];

        unsigned int        freeNodeListCount;
        unsigned int        freeNodes[512];

        unsigned int        seed;
    } eitri_Graph;


    //===================================================

    //inst is the instance of the operation. It contain all info for
    //doing the op (connected input, where to output etc...
    typedef void (*eitri_NodeFunc)(eitri_Graph* graph, int opInst);

    typedef struct
    {
        //how many input that op take
        unsigned char inputImagesCount;

        //this function is called to perform the operation
        eitri_NodeFunc func;

        eitri_NodeParams params[32];
        unsigned int paramsCount;

        //if this is -1, size is dynamic. If set, node only operate in that size*size
        //ATM this is only used by the Color node, @TODO: maybe find a beter way?
        int size;

        char name[256];
    } eitri_Node;

    //===========================================================

    typedef struct
    {
        int         opsCount;
        eitri_Node   ops[1024];
    } eitri_NodesDatabase;

    extern eitri_NodesDatabase eitri_gOpsDB;

    void eitri_init();

    //this is called by the engine on init.
    //it create the map op name <-> associated Op
    //add new ops in that functions
    void eitri_registerOperations();

    void eitri_createGraph(eitri_Graph* g);

    //fill dest with a serialized version (JSON) of the graph (to save/export)
    void eitri_serializeGraph(eitri_Graph* g, char* dest, int maxSize);
    void eitri_deserializeGraph(eitri_Graph* g, const char* data);

    int eitri_addNode(eitri_Graph* g, const char* name);
    void eitri_deleteNode(eitri_Graph* g, int op);

    void eitri_resizeNode(eitri_Graph* g, int node, int newSize);

    //execute the operation, calling recursivly execute on input
    //and stocking result in its _cachedData
    void eitri_doNode(eitri_Graph* g, int op);

    void eitri_connectNode(eitri_Graph* g, int inputOps, int outputOps, int idx);
    void eitri_disconnectNode(eitri_Graph* g, int ops, int idx);

    //generate the cachedData of input named "outputName", and return the index of the nodes (to read the data from embeded)
    int eitri_generateOutput(eitri_Graph* g, const char* outputName);

    //return a pointer to the value which name is given. Allow to modify a param before generating an output.
    // NOTE : only param that where marked as "exposed" can be accessed throught here
    eitri_NodeParamValue* getValue(eitri_Graph* g, const char* name);

    //op here is the op template, not the op instance. This is rarely called by user
    //only in eitri_registerOperations to define all info
    void eitri_addParam(int op, const char* name, const char *tip, eitri_ParamType type);
}
