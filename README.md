dx11framework
=============

My Directx 11 based framework for samples and games. It's "work in progress" so some things are not finished and not cleaned up. At the moment there are no binary or asset files commited so it's impossible to compile and run. For better preview VS 2012 solution and project files are here.

At the moment framework includes basic code for renderer, camera, particles, skinned animations, mesh loading etc.

Intresting parts (they are working but it's still not final version):
- my implementation of shadow mapping using "Rectilinear texture warping" technique (http://broniac.blogspot.com/2012/06/rectilinear-texture-warping-for-fast.html). It uses compute shader and tesselation.
- input system implemented using buffered raw input which is handled using another thread and synchronized using single producer single consumer bounded queue
- simple gpu skinning implementation with loader based on assimp library
- simple particle system using geometry shader
- simple terrain with tesselation
- memory pool and lru cache
- simple, macro-based c++ reflection system + helper class to reflect shaders used to validate cbuffers (not finished)