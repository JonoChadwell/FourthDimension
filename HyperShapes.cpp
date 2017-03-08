#include "HyperShapes.h"

#include "GLSL.h"
#include "Program.h"

#include <vector>
#include <iostream>

using namespace std;

namespace {
    int numbits(int val)
    {
        int rtn = 0;
        while (val) {
            rtn += val & 1;
            val >>= 1;
        }
        return rtn;
    }
}

HyperShape::HyperShape() :
    eleBufID(NULL),
    posBufID(NULL),
    norBufID(NULL),
    vaoID(0)
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

    // Send the normal array to the GPU
    if (norBuf.empty()) {
        norBufID = 0;
    }
    else {
        glGenBuffers(1, &norBufID);
        glBindBuffer(GL_ARRAY_BUFFER, norBufID);
        glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
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
    int h_pos, h_nor;
    h_pos = h_nor = -1;

    glBindVertexArray(vaoID);
    // Bind position buffer
    h_pos = prog->getAttribute("vertPos");
    GLSL::enableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glVertexAttribPointer(h_pos, 4, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    // Bind normal buffer
    h_nor = prog->getAttribute("vertNor");
    if (h_nor != -1 && norBufID != 0) {
        GLSL::enableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, norBufID);
        glVertexAttribPointer(h_nor, 4, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    }

    // Bind element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

    // Draw
    glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);

    // Disable and unbind
    if (h_nor != -1) {
        GLSL::disableVertexAttribArray(h_nor);
    }
    GLSL::disableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

HyperSphere::HyperSphere()
{
}

HyperSphere::~HyperSphere()
{
}

void HyperSphere::load_geometry()
{
    // I have no idea
}

HyperCube::HyperCube()
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

    norBuf = {};
    eleBuf = {};

    for (int i = 0; i < posBuf.size() / 16; i++) {
        norBuf.push_back(1);
        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(0);

        norBuf.push_back(0);
        norBuf.push_back(1);
        norBuf.push_back(0);
        norBuf.push_back(0);

        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(1);

        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(1);
        norBuf.push_back(0);
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

Cube::Cube()
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

    norBuf = {};
    eleBuf = {};

    for (int i = 0; i < posBuf.size() / 16; i++) {
        norBuf.push_back(1);
        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(0);

        norBuf.push_back(0);
        norBuf.push_back(1);
        norBuf.push_back(0);
        norBuf.push_back(0);

        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(1);

        norBuf.push_back(0);
        norBuf.push_back(0);
        norBuf.push_back(1);
        norBuf.push_back(0);
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

Square::Square()
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

    norBuf = {
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
