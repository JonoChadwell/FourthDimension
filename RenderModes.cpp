#include "RenderModes.h"
#include "Program.h"

#include <iostream>

namespace rm {
    Program *wftris;
    Program *wfquads;
    Program *strangesurfaces;
    Program *threedim;

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
        prog->addUniform("wireframeWidth");
        prog->addAttribute("vertPos");
        prog->addAttribute("vertSide");
    }


    void add3dStuff(Program *prog)
    {
        prog->addUniform("PV");
        prog->addUniform("M");
        prog->addUniform("eye");
        prog->addAttribute("vertPos");
        prog->addAttribute("vertNor");
        prog->addAttribute("vertTex");
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

        threedim = new Program();
        threedim->setVerbose(true);
        threedim->setShaderNames("phong_vert.glsl", "phong_frag.glsl");
        threedim->setRenderMode(RENDER_3D);
        threedim->init();
        add3dStuff(threedim);
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
        if (mode == RENDER_3D)
        {
            return threedim;
        }
        std::cerr << "UNKNOWN MODE\n";
        exit(EXIT_FAILURE);
    }
}

