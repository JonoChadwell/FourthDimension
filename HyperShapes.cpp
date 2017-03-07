#include "HyperShapes.h"

#include "GLSL.h"
#include "Program.h"

#include <vector>
#include <iostream>

using namespace std;


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


namespace {

    // varies v1 and v2 while keeping hold constant to produce a surface
    void vary_hold(vector<unsigned int> *buff, int v1, int v2, int hold)
    {
        int a = hold;
        int b = v1 | hold;
        int c = v2 | hold;
        int d = v1 | v2 | hold;

        buff->push_back(a);
        buff->push_back(b);
        buff->push_back(c);
        buff->push_back(a);
        buff->push_back(c);
        buff->push_back(b);

        buff->push_back(b);
        buff->push_back(c);
        buff->push_back(d);
        buff->push_back(b);
        buff->push_back(d);
        buff->push_back(c);
    }

    void create_surfaces(vector<unsigned int> *buff, int a, int b, int c, int d) {
        vary_hold(buff, a, b, 0);
        vary_hold(buff, a, b, c);
        vary_hold(buff, a, b, d);
        vary_hold(buff, a, b, c | d);
    }

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
        1, 1, 1, 1
    };

    norBuf = {
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
        1, 1, 1, 1
    };

    eleBuf = {};

    int x = 0x01;
    int y = 0x02;
    int z = 0x04;
    int w = 0x08;
    
    create_surfaces(&eleBuf, x, y, z, w);
    create_surfaces(&eleBuf, x, z, y, w);
    create_surfaces(&eleBuf, x, w, y, z);
    create_surfaces(&eleBuf, y, z, x, w);
    create_surfaces(&eleBuf, y, w, x, z);
    create_surfaces(&eleBuf, w, z, x, y);
}
