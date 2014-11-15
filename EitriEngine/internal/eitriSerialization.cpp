#include "eitri.h"
#include "eitriInternal.h"

#include "jsmn.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//----- JSON HELPERS

int cmp_json_token(const char* d, jsmntok_t* t, const char* v)
{
    return strncmp(d + t->start, v, (t->end - t->start)) == 0;
}

int json_atoi(const char* d, jsmntok_t* t)
{
    return strtol(d + t->start, NULL, 10);
}

double json_atod(const char* d, jsmntok_t* t)
{
    double ret = strtod(d + t->start, NULL);
    return ret;
}

//this fully "jump" over a node.
//meaning it will descend recursivly inside the node until it passed everytoken
int json_jump_node(jsmntok_t* a, int n)
{
    int tokenToJump = 0;

    int toProcess = 1;
    int current = n;
    do
    {
        toProcess--;

        toProcess += a[current].size;
        tokenToJump += 1;

        current +=1;
    } while(toProcess > 0);

    return tokenToJump;
}

//-----------------------------------

void eitri_serializeGraph(eitri_Graph *g, char *dest, int maxSize)
{
    char* str = dest;

    str += sprintf(str, "{\n\t\"seed\": %i,\n", g->seed);
    str += sprintf(str, "\"nodes\": [ \n");

    for(int i = 0; i < g->operationsCount; ++i)
    {
        str = eitri_saveNode(&g->operations[i], str);

        if(i < g->operationsCount - 1)
            str += sprintf(str, ",\n");
    }

    str += sprintf(str, "]\n");

    str += sprintf(str, "}");
}


char* eitri_saveNode(eitri_OpInstance *inst, char *out)
{
    char* str = out;
    str += sprintf(str, "{\n");
    str += sprintf(str, "\t\"op\":%i,\n", inst->operation);
    str += sprintf(str, "\t\"isOutput\":%i,\n", inst->isOutput);

    str += sprintf(str, "\t\"inputs\":[");
    for(int i = 0; i < 16; ++i)
    {
        str += sprintf(str, "%i", inst->inputs[i]);
        if(i != 15)
            str += sprintf(str, ",");
    }

    str += sprintf(str, "]");

    if(inst->operation != -1)
    {
        eitri_Color tempCol;
        str += sprintf(str, ",\n\"params\":[");
        int nbParam = eitri_gOpsDB.ops[inst->operation].paramsCount;
        for(int i = 0; i < nbParam; ++i)
        {
            switch(eitri_gOpsDB.ops[inst->operation].params[i].type)
            {
            case EITRI_PARAM_FLOAT:
                str += sprintf(str, "%f", inst->paramsValues[i].fParam);
                break;
            case EITRI_PARAM_INT:
                str += sprintf(str, "%i", inst->paramsValues[i].iParam);
                break;
            case EITRI_PARAM_STRING:
                str += sprintf(str, "%s", inst->paramsValues[i].sParam);
                break;
            case EITRI_PARAM_COLOR:
                tempCol = inst->paramsValues[i].colParam;
                str += sprintf(str, "{\"r\":%i, \"g\":%i, \"b\":%i, \"a\":%i }", tempCol.r, tempCol.g, tempCol.b, tempCol.a);
            }

            if(i < nbParam - 1)
                str += sprintf(str, ",\n");
        }

        str += sprintf(str, "]");
    }

    str += sprintf(str, "\n}\n");

    return str;
}

///========


void eitri_deserializeGraph(eitri_Graph *g, const char *data)
{
#define MAX_TOKENS 0x1000
    jsmn_parser parser;
    jsmntok_t tokens[MAX_TOKENS];

    jsmn_init(&parser);
    int nbTokens = jsmn_parse(&parser, data, strlen(data), tokens, MAX_TOKENS);

    //use pos as counter to where we are. kind of a hack but avoid new param
    parser.pos = 1;//we jump the first one, it's the root object

    while(nbTokens > parser.pos)
    {
        jsmntok_t *t = &tokens[parser.pos];
        parser.pos++;

        int length = t->end - t->start;
        if(cmp_json_token(data, t, "seed"))
        {
            t = &tokens[parser.pos];
            parser.pos++;

            g->seed = json_atoi(data, t);
        }
        else if(cmp_json_token(data, t, "nodes"))
        {
            t = &tokens[parser.pos];
            int nb = t->size;//number of entry in the array (nb of nodes)
            parser.pos++;

            g->operationsCount = nb;

            for(int i = 0; i < nb; ++i)
            {//deserialize all nodes
                t = &tokens[parser.pos];
                parser.pos++;

                int length = t->end-t->start;

                eitri_OpInstance* opInst = &g->operations[i];

                //number of data inside the object (the node)
                //divide db y 2 because it's of form, key:value
                int entries = t->size / 2;

                while(entries > 0)
                {
                    t = &tokens[parser.pos];//the key
                    parser.pos++;

                    if(cmp_json_token(data, t, "op"))
                    {
                        t = &tokens[parser.pos];
                        parser.pos++;

                        opInst->operation = json_atoi(data, t);
                        eitri_initOp(g, i,  opInst->operation);
                    }
                    else if(cmp_json_token(data, t, "isOutput"))
                    {
                        t = &tokens[parser.pos];
                        parser.pos++;

                        opInst->isOutput = json_atoi(data, t);
                    }
                    else if(cmp_json_token(data, t, "inputs"))
                    {
                        t = &tokens[parser.pos];
                        parser.pos++;

                        int arrayCount = t->size;

                        for(int k = 0; k < arrayCount; ++k)
                        {//go throught the input array
                            t = &tokens[parser.pos];
                            parser.pos++;

                            opInst->inputs[k] = json_atoi(data, t);
                        }
                    }
                    else if(cmp_json_token(data, t, "params"))
                    {
                        t = &tokens[parser.pos];
                        parser.pos++;

                        int paramCount = t->size;

                        for(int p = 0; p < paramCount; ++p)
                        {
                            t = &tokens[parser.pos];
                            parser.pos++;

                            switch(t->type)
                            {
                            case JSMN_OBJECT://color
                                for(int c = 0; c < 4; ++c)
                                {
                                    t = &tokens[parser.pos];
                                    parser.pos++;
                                    if(cmp_json_token(data, t, "r"))
                                    {
                                        t = &tokens[parser.pos];
                                        parser.pos++;
                                        opInst->paramsValues[p].colParam.r = json_atoi(data, t);
                                    }
                                    else if(cmp_json_token(data, t, "g"))
                                    {
                                        t = &tokens[parser.pos];
                                        parser.pos++;
                                        opInst->paramsValues[p].colParam.g = json_atoi(data, t);
                                    }
                                    else if(cmp_json_token(data, t, "b"))
                                    {
                                        t = &tokens[parser.pos];
                                        parser.pos++;
                                        opInst->paramsValues[p].colParam.b = json_atoi(data, t);
                                    }
                                    else
                                    {
                                        t = &tokens[parser.pos];
                                        parser.pos++;
                                        opInst->paramsValues[p].colParam.a = json_atoi(data, t);
                                    }
                                }
                                break;
                            case JSMN_PRIMITIVE:
                                if(memchr(data + t->start, '.', t->end - t->start) == NULL)
                                {//no . this an integer
                                    opInst->paramsValues[p].iParam = json_atoi(data, t);
                                }
                                else
                                {//this is a float param
                                    opInst->paramsValues[p].fParam = json_atod(data, t);
                                }
                                break;
                             default:
                                break;
                            }
                        }
                    }
                    else
                    {//jump that unknow thing
                        t = &tokens[parser.pos];//the key
                        parser.pos += json_jump_node(tokens, parser.pos);
                    }

                    entries--;
                }
            }


        }
    }
}
