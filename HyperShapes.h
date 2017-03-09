#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Shape.h"

#define RENDER_TRIS_WIREFRAME 1
#define RENDER_QUADS_WIREFRAME 2

#define HYPERSPHERICAL_PARAMETERIZATION 1
#define HOPF_PARAMETERIZATION 2

class Program;

class HyperShape
{
public:
    HyperShape(int renderMode);
    virtual ~HyperShape();
    void init();
    void draw(Program *prog);

protected:
    virtual void load_geometry() = 0;
    std::vector<unsigned int> eleBuf;
    std::vector<float> posBuf;
    std::vector<float> sideBuf;

private:
    int renderMode;
    unsigned eleBufID;
    unsigned posBufID;
    unsigned sideBufID;
    unsigned vaoID;
};


class HyperSphere : public HyperShape
{
public:
    HyperSphere(int parameterization, int divisions);
    ~HyperSphere();

protected:
    void load_geometry();

private:
    int parameterization;
    int divisions;
};

class HyperCube : public HyperShape
{
public:
    HyperCube();
    ~HyperCube();

protected:
    void load_geometry();
};

class Cube : public HyperShape
{
public:
    Cube();
    ~Cube();

protected:
    void load_geometry();
};

class Square : public HyperShape
{
public:
    Square();
    ~Square();

protected:
    void load_geometry();
};

class Simplex : public HyperShape
{
public:
    Simplex(int dimensions);
    ~Simplex();

protected:
    void load_geometry();

private:
    int dimensions;
};

namespace HyperShapes {

    extern HyperShape* hyper_cube;
    extern HyperShape* cube;
    extern HyperShape* square;
    extern HyperShape* triangle;
    extern HyperShape* tetrahedron;
    extern HyperShape* five_cell;
    extern HyperShape* hyper_sphere;

    void initialize();
}