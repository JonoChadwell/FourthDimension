#pragma once

#include <string>
#include <vector>
#include <memory>

class Program;

class Shape
{
public:
    Shape();
    virtual ~Shape();
    void loadMesh(const std::string &meshName);
    void loadMesh(const std::string &meshName, int shape);
    void init();
    void resize();
    void draw(Program *prog);

private:
    std::vector<unsigned int> eleBuf;
    std::vector<float> posBuf;
    std::vector<float> norBuf;
    std::vector<float> texBuf;
    unsigned eleBufID;
    unsigned posBufID;
    unsigned norBufID;
    unsigned texBufID;
    unsigned vaoID;
};
