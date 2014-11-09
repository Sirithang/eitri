#pragma once

extern "C"
{
    typedef struct
    {
        unsigned int w,h;
        unsigned int nbChannel;
        unsigned int channelSize;
    } PicturData;

    //===================================================

    /* Input parameter to an operation
     * Receive the number of input pictures (the op should define that)
     * along with their size and data (a flat array of w*h*nbchannel)
    */
    typedef struct
    {
        unsigned char inputImagesCount;
        void** inputImages;
        PicturData* inputImagesInfos;

        void*  outputImage;
        PicturData outputImageInfo;
    } OpParams;

    typedef void (*opFunc)(OpParams params);

    typedef struct
    {
        //
        unsigned char inputImagesCount;

        //this tell how many channel is inputed from each input
        // -1 (256) mean dynamic input (use the number of the output plug there)
        unsigned char inputChanelCount[256];

        //this function is called to perform the operation
        opFunc func;

        char name[256];
    } Operation;

    //=========================================================

    typedef struct
    {
        char    name[1024];
        int     outputOp;
    } Output;

    //=========================================================

    typedef struct
    {
        int operation;
        int inputs[16];
    } OpInstance;

    //==========================================================

    typedef struct
    {
        unsigned char   outputCount;
        Output          outputs[256];
        unsigned char   outputFreeCount;
        unsigned char   outputFree[256];

        unsigned int    operationsCount;
        OpInstance      operations[2048];

        unsigned int    freeopsListCount;
        unsigned int    freeops[1024];

    }Graph;


    void init();

    //this is called by the engine on init.
    //it create the map op name <-> associated Op
    //add new ops in that functions
    void registerOperations();

    void createGraph(Graph* g);
    int addOutput(Graph* g);

    void addOperation(Graph* g, const char* name);
    int generateOutput(Graph* g, const char* outputName);

    //============== operation impl. =============================

    void outputOp(OpParams params);
}
