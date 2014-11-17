Eitri
=====

Eitri is a simple, no dependency, C lib used to generate procedural texture.
It use a graph of node applying transformation to pixels to get the final result.
It's paired with an editor to create such graph.

It allow to have texture that are only a few kB, since they are juste text graph that generate the texture at runtime (single cost at generation).
Since some parameters can be changed before generation, it also allow to create true "procedural" texture that can be tailored at runtime.

This tool is heavily influenced & inspired by Substance Designer. If you want professional grade tools & support, check : http://www.allegorithmic.com/products/substance-designer

**NOTE** : current version is a pre-alpha made for the procjam. It was made in less than a week during evening, so it's terribly buggy and lacking features. Give it a spin if you feel adventurous though, and report feedback/wishes!

**Use release on top of the source (next to branch) to download a prebuilt windows version + dependencies + lib/header for embedding** 

Editor
=====

![Eitri screenshot](screenshot.png?raw=true "Editor screenshot")

The editor is the first step for the texture creation. It's a node-based graph (know by artists as it's used in shaders editors for example) that define a series of transformation to applies to images (generated or imported) toward a named output.

That graph can then be saved to be used in the embeddable engine, or output can be exported to simple image to be used in any other applications as any other images.

Nodes have parameters and some can be "exposed" to be modified in C by the user.

To add a node, press *Space* inside the graph display, and choose in the list the one you want (double click to add)

Library
=====

The lib is a simple C lib (for FFI use in any engine) having no external dependency.

It take a graph (define as a JSON file, created using the Editor) that it load in memory.

Users can then query the for a specific output and the lib will return a buffer containing the generated image.

If the graph creator have exposed parameters, they can be set before asking for the end picture, changing the output.

Use example in engine

```C++
eitri_init();
eitri_Graph g = new eitri_Graph(); //need to be dynamic, 1 inst == 8Mb
eitri_deserializeGraph(g, fileContent); //fileContent is the file content of a .eit file
int nodeIdx = eitri_generateOutput(g, "diffuse"); //is the graph have an output named "diffuse"

eitri_NodeInstance* node = &g->nodes[nodeIdx];
node->_cachedData.data;//contain the bitmap data
node->_cachedData.w;//contain width of image. Same with h
```


Building
======

The lib try to have no dependency other than itself. The source code can be compiled however you want, and your application only have to include "eitri.h" located in EitriEngine/include

Building
========

EitriEngine contains the engine to parse a graph and generate an output.
Eitri contains the application to generate and export the graph.

Eitri depends on EitriEngine.lib & Qt 5.3 (superior version should also work)
EitriEngine depends on nothing (apart from c compiler).

Project in the repo are for Qt Creator (so that it should be portable more easily). Just need to add the relevant info to link against EitriEngine in Eitri.
