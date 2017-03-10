#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Shape.h"
#include "RenderModes.h"

#define HYPERSPHERICAL_PARAMETERIZATION 1
// Fix parameterization before use
#define HOPF_PARAMETERIZATION 2

class Program;

class HyperShape
{
public:
    HyperShape(int renderMode);
    virtual ~HyperShape();
    void init();
    void draw(Program *prog);
    int defaultRenderMode;

protected:
    virtual void load_geometry() = 0;
    std::vector<unsigned int> eleBuf;
    std::vector<float> posBuf;
    std::vector<float> sideBuf;

private:
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
    extern HyperShape* hyper_sphere_lq;
    extern HyperShape* hyper_sphere;
    extern HyperShape* hyper_sphere_hq;

    void initialize();
}