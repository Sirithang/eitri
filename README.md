Eitri
=====

Eitri is a simple, no dependency, C lib used to generate procedural texture.
It use a graph of node applying transformation to pixels to get the final result.
It's paired with an editor to create such graph.

It allow to have texture that are only a few kB, since they are juste text graph that generate the texture at runtime (single cost at generation).
Since some parameters can be changed before generation, it also allow to create true "procedural" texture that can be tailored at runtime.

This tool is heavily influenced & inspired by Substance Designer. If you want professional grade tools & support, check : http://www.allegorithmic.com/products/substance-designer

Editor
=====

The editor is the first step for the texture creation. It's a node-based graph (know by artists as it's used in shaders editors for example) that define a series of transformation to applies to images (generated or imported) toward a named output.

That graph can then be saved to be used in the embeddable engine, or output can be exported to simple image to be used in any other applications as any other images.

Nodes have parameters and some can be "exposed" to be modified in C by the user.

Library
=====

The lib is a simple C lib (for FFI use in any engine) having no external dependency.

It take a graph (define as a JSON file, created using the Editor) that it load in memory.

Users can then query the for a specific output and the lib will return a buffer containing the generated image.

If the graph creator have exposed parameters, they can be set before asking for the end picture, changing the output.


Building
======

The lib try to have no dependency other than itself. The source code can be compiled however you want, and your application only have to