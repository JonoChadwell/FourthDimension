#pragma once

#define RENDER_MODE unsigned int
#define RENDER_OTHER 0
#define RENDER_TRIS_WIREFRAME 1
#define RENDER_QUADS_WIREFRAME 2
#define RENDER_STRANGE_COLORED 3

class Program;

namespace rm {
    void init();
    Program* getProgram(RENDER_MODE mode);
}
