#ifndef EITRIINTERNAL_H
#define EITRIINTERNAL_H

#include "eitri.h"

//===========================================================

void eitri_initOp(eitri_Graph *g, int idx, int opIdx);

char* eitri_saveNode(eitri_NodeInstance* inst, char* out);


//============== operation impl. =============================

void eitri_op_output(eitri_Graph* graph, int opInst);
void eitri_op_noise(eitri_Graph* graph,  int opInst);
void eitri_op_color(eitri_Graph* graph, int opInst);
void eitri_op_multiply(eitri_Graph* graph, int opInst);

void eitri_op_maskblend(eitri_Graph* graph, int opInstance);

void eitri_op_brick(eitri_Graph* graph, int opInst);
void eitri_op_perlin(eitri_Graph* graph, int opInst);
void eitri_op_normalmap(eitri_Graph* graph, int opInst);

//=========== op helpers =================================



#endif // EITRIINTERNAL_H
