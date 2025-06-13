#pragma once

#include <vector>
#include <set> // Required for std::set in QueryRay
#include <algorithm> // Required for std::min/max
#include <cmath>     // Required for std::fabs, std::floor, sqrt
#include <float.h>   // Required for FLT_MAX

// Forward declare sObject if its full definition is not needed here
// or include the header that defines it (assuming it's in "CObjectsC.h" or "gameguru.h")
#include "globstruct.h" // Attempting to include what might contain sObject and GGVECTOR3 more directly.
                        // If "gameguru.h" is too large, this might be a subset.
                        // If sObject is in CObjectsC.h, that will be included by SpatialPartition.cpp

// If GGVECTOR3 is not in globstruct.h, it's likely in "cVectorC.h" or similar
// For now, assume globstruct.h or includes within gameguru.h (via SpatialPartition.cpp) will provide it.

class CSpatialPartition
{
public:
    CSpatialPartition();
    ~CSpatialPartition();

    // Using world min/max coordinates for initialization
    void Init(float worldMinX, float worldMinY, float worldMinZ,
              float worldMaxX, float worldMaxY, float worldMaxZ,
              int divisionsX, int divisionsY, int divisionsZ);

    void AddObject(sObject* pObject);
    void RemoveObject(sObject* pObject);
    std::vector<sObject*> QueryRay(float startX, float startY, float startZ, float endX, float endY, float endZ);

    // void DebugRender(); // Optional

private:
    struct Cell
    {
        std::vector<sObject*> objects;
    };

    std::vector<std::vector<std::vector<Cell>>> m_grid;

    GGVECTOR3 m_origin; // World coordinate of the grid's minimum corner
    float m_worldSizeX, m_worldSizeY, m_worldSizeZ;
    int m_divisionsX, m_divisionsY, m_divisionsZ;
    float m_cellSizeX, m_cellSizeY, m_cellSizeZ;

    void GetCellIndices(float x, float y, float z, int& ix, int& iy, int& iz) const;
};
