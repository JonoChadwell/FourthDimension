#include "RenderModes.h"
#include "Program.h"

#include <iostream>

namespace rm {
    Program *wftris;
    Program *wfquads;
    Program *strangesurfaces;

    void add4dStuff(Program *prog)
    {
        prog->addUniform("PV");
        prog->addUniform("M");
        prog->addUniform("Q");
        prog->addUniform("R");
        prog->addUniform("objPos");
        prog->addUniform("divW");
        prog->addUniform("sliceWidth");
        prog->addUniform("sliceOffset");
        prog->addAttribute("vertPos");
        prog->addAttribute("vertSide");
    }

    void init()
    {
        wftris = new Program();
        wftris->setVerbose(true);
        wftris->setShaderNames("geom_vertex_shader.glsl", "wireframe_tris_frag.glsl");
        wftris->setRenderMode(RENDER_TRIS_WIREFRAME);
        wftris->init();
        add4dStuff(wftris);

        wfquads = new Program();
        wfquads->setVerbose(true);
        wfquads->setShaderNames("geom_vertex_shader.glsl", "wireframe_quads_frag.glsl");
        wfquads->setRenderMode(RENDER_QUADS_WIREFRAME);
        wfquads->init();
        add4dStuff(wfquads);

        strangesurfaces = new Program();
        strangesurfaces->setVerbose(true);
        strangesurfaces->setShaderNames("geom_vertex_shader.glsl", "strange_colored_frag.glsl");
        strangesurfaces->setRenderMode(RENDER_STRANGE_COLORED);
        strangesurfaces->init();
        add4dStuff(strangesurfaces);
    }

    Program* getProgram(RENDER_MODE mode)
    {
        if (mode == RENDER_TRIS_WIREFRAME)
        {
            return wftris;
        }
        if (mode == RENDER_QUADS_WIREFRAME)
        {
            return wfquads;
        }
        if (mode == RENDER_STRANGE_COLORED)
        {
            return strangesurfaces;
        }
        std::cerr << "UNKNOWN MODE\n";
        exit(EXIT_FAILURE);
    }
}

