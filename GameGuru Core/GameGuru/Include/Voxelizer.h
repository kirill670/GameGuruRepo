#pragma once

#include <vector>
#include "cVectorC.h"

class Voxelizer
{
public:
    Voxelizer(int resolution);
    ~Voxelizer();

    void voxelize(const std::vector<int>& objectIDs);
    const std::vector<int>& getVoxelData() const { return voxelData; }

private:
    void rasterizeTriangle(const cVector3& v0, const cVector3& v1, const cVector3& v2);
    bool isPointInTriangle(const cVector3& point, const cVector3& v0, const cVector3& v1, const cVector3& v2);

    int resolution;
    std::vector<int> voxelData;
};
