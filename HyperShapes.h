#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Shape.h"

class Program;

class HyperShape
{
public:
    HyperShape();
    virtual ~HyperShape();
    void init();
    void draw(Program *prog);

protected:
    virtual void load_geometry() = 0;
    std::vector<unsigned int> eleBuf;
    std::vector<float> posBuf;
    std::vector<float> norBuf;

private:
    unsigned eleBufID;
    unsigned posBufID;
    unsigned norBufID;
    unsigned vaoID;
};


class HyperSphere : public HyperShape
{
public:
    HyperSphere();
    ~HyperSphere();

protected:
    void load_geometry();
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
