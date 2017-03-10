#include "HyperShapes.h"

#include "GLSL.h"
#include "Program.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <cmath>
#include <iostream>

struct test {
    bool a;
    int b;
};

using namespace std;

namespace {

    void push_quad_side(vector<float> *sideBuf) {
        sideBuf->push_back(1);
        sideBuf->push_back(0);
        sideBuf->push_back(0);
        sideBuf->push_back(0);

        sideBuf->push_back(0);
        sideBuf->push_back(1);
        sideBuf->push_back(0);
        sideBuf->push_back(0);

        sideBuf->push_back(0);
        sideBuf->push_back(0);
        sideBuf->push_back(0);
        sideBuf->push_back(1);

        sideBuf->push_back(0);
        sideBuf->push_back(0);
        sideBuf->push_back(1);
        sideBuf->push_back(0);
    }

    int numbits(int val)
    {
        int rtn = 0;
        while (val) {
            rtn += val & 1;
            val >>= 1;
        }
        return rtn;
    }

    void push_hs_point(vector<float> *pos, int r1, int r2, int r3, int divisions, int mode)
    {
        float x, y, z, w;

        if (mode == HYPERSPHERICAL_PARAMETERIZATION) {
            float psi = ((float)r1 / (float)divisions) * M_PI;
            float theta = ((float)r2 / (float)divisions) * M_PI;
            float phi = ((float)r3 / (float)divisions) * M_PI;

            x = cos(psi);
            y = sin(psi) * cos(theta);
            z = sin(psi) * sin(theta) * cos(phi);
            w = sin(psi) * sin(theta) * sin(phi);
        }
        else if (mode == HOPF_PARAMETERIZATION)
        {
            float e1 = ((float)r1 / (float)divisions) * 2 * M_PI;
            float e2 = ((float)r2 / (float)divisions) * 2 * M_PI;
            float n = ((float)r3 / (float)divisions) * M_PI / 2;

            x = sin(e1) * sin(n);
            y = cos(e1) * sin(n);
            z = sin(e2) * cos(n);
            w = cos(e2) * cos(n);
        }
        else
        {
            cerr << "Invalid hypersphere parameterization\n";
            exit(EXIT_FAILURE);
        }

        pos->push_back(x);
        pos->push_back(y);
        pos->push_back(z);
        pos->push_back(w);
    }
}

HyperShape::HyperShape(int renderMode) :
    eleBufID(NULL),
    posBufID(NULL),
    sideBufID(NULL),
    vaoID(0),
    defaultRenderMode(renderMode)
{
}

HyperShape::~HyperShape()
{
}

void HyperShape::init()
{
    load_geometry();

    // Initialize the vertex array object
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    // Send the position array to the GPU
    glGenBuffers(1, &posBufID);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);

    // Send the side array to the GPU
    if (sideBuf.empty()) {
        sideBufID = 0;
    }
    else {
        glGenBuffers(1, &sideBufID);
        glBindBuffer(GL_ARRAY_BUFFER, sideBufID);
        glBufferData(GL_ARRAY_BUFFER, sideBuf.size() * sizeof(float), &sideBuf[0], GL_STATIC_DRAW);
    }

    // Send the element array to the GPU
    glGenBuffers(1, &eleBufID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size() * sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);

    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (glGetError() != GL_NO_ERROR)
    {
        exit(EXIT_FAILURE);
    }
}

void HyperShape::draw(Program *prog)
{
    int h_pos, h_side;
    h_pos = h_side = -1;

    glBindVertexArray(vaoID);
    // Bind position buffer
    h_pos = prog->getAttribute("vertPos");
    GLSL::enableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glVertexAttribPointer(h_pos, 4, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    // Bind side buffer
    h_side = prog->getAttribute("vertSide");
    if (h_side != -1 && sideBufID != 0) {
        GLSL::enableVertexAttribArray(h_side);
        glBindBuffer(GL_ARRAY_BUFFER, sideBufID);
        glVertexAttribPointer(h_side, 4, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    }

    // Bind element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

    // Draw
    glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);

    // Disable and unbind
    if (h_side != -1) {
        GLSL::disableVertexAttribArray(h_side);
    }
    GLSL::disableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

HyperSphere::HyperSphere(int parameterization, int divisions) :
    HyperShape(RENDER_QUADS_WIREFRAME),
    parameterization(parameterization),
    divisions(divisions)
{
}

HyperSphere::~HyperSphere()
{
}



void HyperSphere::load_geometry()
{
    posBuf = {};
    sideBuf = {};
    eleBuf = {};

    int verticies = 0;

    for (int r1 = 0; r1 < divisions; r1++) {
        for (int r2 = 0; r2 < divisions; r2++) {
            for (int r3 = 0; r3 < divisions * 2; r3++) {
                // XY quads
                push_hs_point(&posBuf, r1, r2, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2 + 1, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2 + 1, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2 + 1, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2 + 1, r3 + 1, divisions, parameterization);

                // XZ quads
                push_hs_point(&posBuf, r1, r2, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2 + 1, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2 + 1, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2 + 1, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2 + 1, r3 + 1, divisions, parameterization);

                // YZ quads
                push_hs_point(&posBuf, r1, r2, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2 + 1, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1, r2 + 1, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2 + 1, r3, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2, r3 + 1, divisions, parameterization);
                push_hs_point(&posBuf, r1 + 1, r2 + 1, r3 + 1, divisions, parameterization);
            }
        }
    }

    for (int i = 0; i < posBuf.size() / 16; i++) {
        push_quad_side(&sideBuf);
    }

    for (int i = 0; i < posBuf.size() / 16; i++) {
        eleBuf.push_back(4 * i + 0);
        eleBuf.push_back(4 * i + 1);
        eleBuf.push_back(4 * i + 2);

        eleBuf.push_back(4 * i + 1);
        eleBuf.push_back(4 * i + 2);
        eleBuf.push_back(4 * i + 3);
    }
}

HyperCube::HyperCube() :
    HyperShape(RENDER_QUADS_WIREFRAME)
{
}

HyperCube::~HyperCube()
{
}

void HyperCube::load_geometry()
{
    posBuf = {
        // zw surfaces
        -1, -1, -1, -1,
        -1, -1, -1, 1,
        -1, -1, 1, -1,
        -1, -1, 1, 1,
        -1, 1, -1, -1,
        -1, 1, -1, 1,
        -1, 1, 1, -1,
        -1, 1, 1, 1,
        1, -1, -1, -1,
        1, -1, -1, 1,
        1, -1, 1, -1,
        1, -1, 1, 1,
        1, 1, -1, -1,
        1, 1, -1, 1,
        1, 1, 1, -1,
        1, 1, 1, 1,

        // yw surfaces
        -1, -1, -1, -1,
        -1, -1, -1, 1,
        -1, 1, -1, -1,
        -1, 1, -1, 1,
        -1, -1, 1, -1,
        -1, -1, 1, 1,
        -1, 1, 1, -1,
        -1, 1, 1, 1,
        1, -1, -1, -1,
        1, -1, -1, 1,
        1, 1, -1, -1,
        1, 1, -1, 1,
        1, -1, 1, -1,
        1, -1, 1, 1,
        1, 1, 1, -1,
        1, 1, 1, 1,
        
        // xw surfaces
        -1, -1, -1, -1,
        -1, -1, -1, 1,
        1, -1, -1, -1,
        1, -1, -1, 1,
        -1, -1, 1, -1,
        -1, -1, 1, 1,
        1, -1, 1, -1,
        1, -1, 1, 1,
        -1, 1, -1, -1,
        -1, 1, -1, 1,
        1, 1, -1, -1,
        1, 1, -1, 1,
        -1, 1, 1, -1,
        -1, 1, 1, 1,
        1, 1, 1, -1,
        1, 1, 1, 1,

        // xz surfaces
        -1, -1, -1, -1,
        -1, -1, 1, -1,
        1, -1, -1, -1,
        1, -1, 1, -1,
        -1, -1, -1, 1,
        -1, -1, 1, 1,
        1, -1, -1, 1,
        1, -1, 1, 1,
        -1, 1, -1, -1,
        -1, 1, 1, -1,
        1, 1, -1, -1,
        1, 1, 1, -1,
        -1, 1, -1, 1,
        -1, 1, 1, 1,
        1, 1, -1, 1,
        1, 1, 1, 1,
       
        // yz surfaces
        -1, -1, -1, -1,
        -1, -1, 1, -1,
        -1, 1, -1, -1,
        -1, 1, 1, -1,
        -1, -1, -1, 1,
        -1, -1, 1, 1,
        -1, 1, -1, 1,
        -1, 1, 1, 1,
        1, -1, -1, -1,
        1, -1, 1, -1,
        1, 1, -1, -1,
        1, 1, 1, -1,
        1, -1, -1, 1,
        1, -1, 1, 1,
        1, 1, -1, 1,
        1, 1, 1, 1,

        // xy surfaces
        -1, -1, -1, -1,
        -1, 1, -1, -1,
        1, -1, -1, -1,
        1, 1, -1, -1,
        -1, -1, -1, 1,
        -1, 1, -1, 1,
        1, -1, -1, 1,
        1, 1, -1, 1,
        -1, -1, 1, -1,
        -1, 1, 1, -1,
        1, -1, 1, -1,
        1, 1, 1, -1,
        -1, -1, 1, 1,
        -1, 1, 1, 1,
        1, -1, 1, 1,
        1, 1, 1, 1,
    };

    sideBuf = {};
    eleBuf = {};

    for (int i = 0; i < posBuf.size() / 16; i++) {
        push_quad_side(&sideBuf);
    }

    for (int i = 0; i < posBuf.size() / 16; i++) {
        eleBuf.push_back(4 * i + 0);
        eleBuf.push_back(4 * i + 1);
        eleBuf.push_back(4 * i + 2);

        eleBuf.push_back(4 * i + 1);
        eleBuf.push_back(4 * i + 2);
        eleBuf.push_back(4 * i + 3);
    }
}

Cube::Cube() :
    HyperShape(RENDER_QUADS_WIREFRAME)
{
}

Cube::~Cube()
{
}

void Cube::load_geometry()
{
    posBuf = {
        // xz surfaces
        -1, -1, -1, 0,
        -1, -1, 1, 0,
        1, -1, -1, 0,
        1, -1, 1, 0,
        -1, 1, -1, 0,
        -1, 1, 1, 0,
        1, 1, -1, 0,
        1, 1, 1, 0,

        // yz surfaces
        -1, -1, -1, 0,
        -1, -1, 1, 0,
        -1, 1, -1, 0,
        -1, 1, 1, 0,
        1, -1, -1, 0,
        1, -1, 1, 0,
        1, 1, -1, 0,
        1, 1, 1, 0,

        // xy surfaces
        -1, -1, -1, 0,
        -1, 1, -1, 0,
        1, -1, -1, 0,
        1, 1, -1, 0,
        -1, -1, 1, 0,
        -1, 1, 1, 0,
        1, -1, 1, 0,
        1, 1, 1, 0,
    };

    sideBuf = {};
    eleBuf = {};

    for (int i = 0; i < posBuf.size() / 16; i++) {
        push_quad_side(&sideBuf);
    }

    for (int i = 0; i < posBuf.size() / 16; i++) {
        eleBuf.push_back(4 * i + 0);
        eleBuf.push_back(4 * i + 1);
        eleBuf.push_back(4 * i + 2);

        eleBuf.push_back(4 * i + 1);
        eleBuf.push_back(4 * i + 2);
        eleBuf.push_back(4 * i + 3);
    }
}

Square::Square() :
    HyperShape(RENDER_QUADS_WIREFRAME)
{
}

Square::~Square()
{
}

void Square::load_geometry()
{
    posBuf = {
        -1, -1, 0, 0,
        -1, 1, 0, 0,
        1, -1, 0, 0,
        1, 1, 0, 0
    };

    sideBuf = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 1,
        0, 0, 1, 0,
    };

    eleBuf = {
        0, 1, 2,
        1, 2, 3
    };
}

Simplex::Simplex(int dimensions) :
    HyperShape(RENDER_TRIS_WIREFRAME),
    dimensions(dimensions)
{
    if (dimensions < 2 || dimensions > 4)
    {
        cerr << dimensions << "-Simplex not supported" << endl;
        exit(EXIT_FAILURE);
    }
}

Simplex::~Simplex()
{
}

vector<vector<vector<float>>> simplex_verticies = 
{
// 0-simplex
{},
// 1-simplex
{},
// 2-simplex (triangle)
{
    {0.0f, 1.0f, 0, 0},
    {0.8660254f, -0.5f, 0, 0},
    {-0.8660254f, -0.5f, 0, 0},
},
// 3-simplex (tetrahedron)
{
    {1.0f, 1, 1, 0},
    {1, -1, -1, 0},
    {-1, 1, -1, 0},
    {-1, -1, 1, 0},
},
// 4-simplex (5-cell)
{
    {1 / sqrt(10.0f), 1 / sqrt(6.0f), 1 / sqrt(3.0f), 1},
    {1 / sqrt(10.0f), 1 / sqrt(6.0f), 1 / sqrt(3.0f), -1},
    {1 / sqrt(10.0f), 1 / sqrt(6.0f), -2 / sqrt(3.0f), 0},
    {1 / sqrt(10.0f), -sqrt(3.0f / 2.0f), 0, 0},
    {-2 * sqrt(2.0f / 5.0f), 0, 0, 0},
},
};

void Simplex::load_geometry()
{
    posBuf = {};
    sideBuf = {};
    eleBuf = {};

    int verticies = 0;

    for (int c1 = 0; c1 <= dimensions; c1++) {
        for (int c2 = c1 + 1; c2 <= dimensions; c2++) {
            for (int c3 = c2 + 1; c3 <= dimensions; c3++) {
                for (int i = 0; i < 4; i++) {
                    posBuf.push_back(simplex_verticies[dimensions][c1][i]);
                }
                for (int i = 0; i < 4; i++) {
                    posBuf.push_back(simplex_verticies[dimensions][c2][i]);
                }
                for (int i = 0; i < 4; i++) {
                    posBuf.push_back(simplex_verticies[dimensions][c3][i]);
                }

                sideBuf.push_back(1);
                sideBuf.push_back(0);
                sideBuf.push_back(0);
                sideBuf.push_back(0);

                sideBuf.push_back(0);
                sideBuf.push_back(1);
                sideBuf.push_back(0);
                sideBuf.push_back(0);

                sideBuf.push_back(0);
                sideBuf.push_back(0);
                sideBuf.push_back(1);
                sideBuf.push_back(0);

                eleBuf.push_back(verticies++);
                eleBuf.push_back(verticies++);
                eleBuf.push_back(verticies++);
            }
        }
    }
}

namespace HyperShapes {
    HyperShape* hyper_cube;
    HyperShape* cube;
    HyperShape* square;
    HyperShape* triangle;
    HyperShape* tetrahedron;
    HyperShape* five_cell;
    HyperShape* hyper_sphere;
    HyperShape* hyper_sphere_lq;
    HyperShape* hyper_sphere_hq;

    void initialize()
    {
        cout << "Initializing Geometry\n";

        HyperShapes::hyper_cube = new HyperCube();
        HyperShapes::hyper_cube->init();

        HyperShapes::cube = new Cube();
        HyperShapes::cube->init();

        HyperShapes::square = new Square();
        HyperShapes::square->init();

        HyperShapes::triangle = new Simplex(2);
        HyperShapes::triangle->init();

        HyperShapes::tetrahedron = new Simplex(3);
        HyperShapes::tetrahedron->init();

        HyperShapes::five_cell = new Simplex(4);
        HyperShapes::five_cell->init();

        HyperShapes::hyper_sphere_lq = new HyperSphere(HYPERSPHERICAL_PARAMETERIZATION, 6);
        HyperShapes::hyper_sphere_lq->init();

        HyperShapes::hyper_sphere = new HyperSphere(HYPERSPHERICAL_PARAMETERIZATION, 15);
        HyperShapes::hyper_sphere->init();

        HyperShapes::hyper_sphere_hq = new HyperSphere(HYPERSPHERICAL_PARAMETERIZATION, 30);
        HyperShapes::hyper_sphere_hq->init();

        cout << "Initialized Geometry\n";
    }
}


