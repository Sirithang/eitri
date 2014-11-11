#pragma once

extern "C"
{
    typedef struct
    {
        unsigned int w,h;
        unsigned int nbChannel;
        unsigned int channelSize;
    } eitri_PicturData;

    //===================================================

    /* Input parameter to an operation
     * Receive the number of input pictures (the op should define that)
     * along with their size and data (a flat array of w*h*nbchannel)
    */
    typedef struct
    {
        unsigned char inputImagesCount;
        void** inputImages;
        eitri_PicturData* inputImagesInfos;

        void*  outputImage;
        eitri_PicturData outputImageInfo;
    } eitri_OpFuncParams;

    typedef void (*eitri_opFunc)(eitri_OpFuncParams params);

    typedef struct
    {
        //
        unsigned char inputImagesCount;

        //this tell how many channel is inputed from each input
        // -1 (256) mean dynamic input (use the number of the output plug there)
        unsigned char inputChanelCount[256];

        //this function is called to perform the operation
        eitri_opFunc func;

        char name[256];
    } eitri_Operation;

    //=========================================================

    typedef struct
    {
        char    name[1024];
        int     outputOp;
    } eitri_Output;

    //=========================================================

    typedef struct
    {
        int operation;
        int inputs[16];
    } eitri_OpInstance;

    //==========================================================

    typedef struct
    {
        unsigned char       outputCount;
        eitri_Output        outputs[256];
        unsigned char       outputFreeCount;
        unsigned char       outputFree[256];

        unsigned int        operationsCount;
        eitri_OpInstance    operations[2048];

        unsigned int        freeopsListCount;
        unsigned int        freeops[1024];

    }eitri_Graph;


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
    int eitri_addOutput(eitri_Graph* g);

    int eitri_addOperation(eitri_Graph* g, const char* name);
    int eitri_generateOutput(eitri_Graph* g, const char* outputName);

    //============== operation impl. =============================

    void eitri_noiseOp(eitri_OpFuncParams params);
}
