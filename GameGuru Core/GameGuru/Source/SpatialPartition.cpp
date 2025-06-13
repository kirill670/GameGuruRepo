#include "stdafx.h"
#include "SpatialPartition.h"
#include "CObjectsC.h" // For sObject definition, ObjectPositionX/Y/Z, ObjectSizeX/Y/Z
#include "gameguru.h"   // For GGVECTOR3, GGVec3Length, GGVec3Normalize, etc. (or their underlying headers)
                       // Also for t.terrain if used for world bounds in G-Entity.cpp

// Constructor
CSpatialPartition::CSpatialPartition()
    : m_worldSizeX(0), m_worldSizeY(0), m_worldSizeZ(0),
      m_divisionsX(0), m_divisionsY(0), m_divisionsZ(0),
      m_cellSizeX(0), m_cellSizeY(0), m_cellSizeZ(0)
{
    m_origin = GGVECTOR3(0.0f, 0.0f, 0.0f);
}

// Destructor
CSpatialPartition::~CSpatialPartition()
{
    // Grid cells own sObject pointers but don't delete them, owner is elsewhere
}

// Initialize the spatial partition
void CSpatialPartition::Init(float worldMinX, float worldMinY, float worldMinZ,
                           float worldMaxX, float worldMaxY, float worldMaxZ,
                           int divisionsX, int divisionsY, int divisionsZ)
{
    m_origin = GGVECTOR3(worldMinX, worldMinY, worldMinZ);
    m_worldSizeX = worldMaxX - worldMinX;
    m_worldSizeY = worldMaxY - worldMinY;
    m_worldSizeZ = worldMaxZ - worldMinZ;

    m_divisionsX = divisionsX > 0 ? divisionsX : 1;
    m_divisionsY = divisionsY > 0 ? divisionsY : 1;
    m_divisionsZ = divisionsZ > 0 ? divisionsZ : 1;

    // Ensure cell sizes are not zero if world size in a dimension is zero
    m_cellSizeX = (m_worldSizeX > 0.0f && m_divisionsX > 0) ? m_worldSizeX / m_divisionsX : 1.0f;
    m_cellSizeY = (m_worldSizeY > 0.0f && m_divisionsY > 0) ? m_worldSizeY / m_divisionsY : 1.0f;
    m_cellSizeZ = (m_worldSizeZ > 0.0f && m_divisionsZ > 0) ? m_worldSizeZ / m_divisionsZ : 1.0f;
    if (m_cellSizeX < 1.0f) m_cellSizeX = 1.0f; // Minimum cell size
    if (m_cellSizeY < 1.0f) m_cellSizeY = 1.0f;
    if (m_cellSizeZ < 1.0f) m_cellSizeZ = 1.0f;


    m_grid.assign(m_divisionsX, std::vector<std::vector<Cell>>(m_divisionsY, std::vector<Cell>(m_divisionsZ)));
}

// Helper function to get cell indices from world coordinates
void CSpatialPartition::GetCellIndices(float x, float y, float z, int& ix, int& iy, int& iz) const
{
    float relX = x - m_origin.x;
    float relY = y - m_origin.y;
    float relZ = z - m_origin.z;

    ix = static_cast<int>(std::floor(relX / m_cellSizeX));
    iy = static_cast<int>(std::floor(relY / m_cellSizeY));
    iz = static_cast<int>(std::floor(relZ / m_cellSizeZ));

    ix = std::max(0, std::min(ix, m_divisionsX - 1));
    iy = std::max(0, std::min(iy, m_divisionsY - 1));
    iz = std::max(0, std::min(iz, m_divisionsZ - 1));
}

void CSpatialPartition::AddObject(sObject* pObject)
{
    if (!pObject || pObject->iID <= 0) return;
    // Ensure pFrame is valid if accessing collision data that might depend on it.
    // Most sObject fields should be valid if pObject itself is valid.

    GGVECTOR3 center;
    float halfSizeX, halfSizeY, halfSizeZ;

    // Use the object's specific collision box data if valid, otherwise use overall object size
    // This assumes pObject->collision data is up-to-date world coordinates or relative and needs transform
    // For now, assume pObject->collision.vecMin/Max are world AABB if available, else derive from position/size
    if (pObject->collision.bActive && pObject->collision.bColBox &&
        pObject->collision.vecMax.x > pObject->collision.vecMin.x) // Check if collision box is valid
    {
        minBound = pObject->collision.vecMin;
        maxBound = pObject->collision.vecMax;
    }
    else // Fallback to overall object size and position
    {
        center = GGVECTOR3(ObjectPositionX(pObject->iID), ObjectPositionY(pObject->iID), ObjectPositionZ(pObject->iID));
        halfSizeX = ObjectSizeX(pObject->iID, true) / 2.0f; // true for scaled size
        halfSizeY = ObjectSizeY(pObject->iID, true) / 2.0f;
        halfSizeZ = ObjectSizeZ(pObject->iID, true) / 2.0f;
        if (halfSizeX < 1.0f) halfSizeX = 1.0f; // Minimum extent
        if (halfSizeY < 1.0f) halfSizeY = 1.0f;
        if (halfSizeZ < 1.0f) halfSizeZ = 1.0f;
        minBound = GGVECTOR3(center.x - halfSizeX, center.y - halfSizeY, center.z - halfSizeZ);
        maxBound = GGVECTOR3(center.x + halfSizeX, center.y + halfSizeY, center.z + halfSizeZ);
    }

    int minIX, minIY, minIZ;
    int maxIX, maxIY, maxIZ;

    GetCellIndices(minBound.x, minBound.y, minBound.z, minIX, minIY, minIZ);
    GetCellIndices(maxBound.x, maxBound.y, maxBound.z, maxIX, maxIY, maxIZ);

    for (int ix = minIX; ix <= maxIX; ++ix)
    {
        if (ix < 0 || ix >= m_divisionsX) continue;
        for (int iy = minIY; iy <= maxIY; ++iy)
        {
            if (iy < 0 || iy >= m_divisionsY) continue;
            for (int iz = minIZ; iz <= maxIZ; ++iz)
            {
                if (iz < 0 || iz >= m_divisionsZ) continue;

                Cell& cell = m_grid[ix][iy][iz];
                bool found = false;
                for(sObject* pCurrentObj : cell.objects) {
                    if(pCurrentObj == pObject) {
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    cell.objects.push_back(pObject);
                }
            }
        }
    }
}

void CSpatialPartition::RemoveObject(sObject* pObject)
{
    if (!pObject || pObject->iID <= 0) return;

    GGVECTOR3 minBound, maxBound, center;
    float halfSizeX, halfSizeY, halfSizeZ;

    if (pObject->collision.bActive && pObject->collision.bColBox &&
        pObject->collision.vecMax.x > pObject->collision.vecMin.x)
    {
        minBound = pObject->collision.vecMin;
        maxBound = pObject->collision.vecMax;
    }
    else
    {
        center = GGVECTOR3(ObjectPositionX(pObject->iID), ObjectPositionY(pObject->iID), ObjectPositionZ(pObject->iID));
        halfSizeX = ObjectSizeX(pObject->iID, true) / 2.0f;
        halfSizeY = ObjectSizeY(pObject->iID, true) / 2.0f;
        halfSizeZ = ObjectSizeZ(pObject->iID, true) / 2.0f;
        if (halfSizeX < 1.0f) halfSizeX = 1.0f;
        if (halfSizeY < 1.0f) halfSizeY = 1.0f;
        if (halfSizeZ < 1.0f) halfSizeZ = 1.0f;
        minBound = GGVECTOR3(center.x - halfSizeX, center.y - halfSizeY, center.z - halfSizeZ);
        maxBound = GGVECTOR3(center.x + halfSizeX, center.y + halfSizeY, center.z + halfSizeZ);
    }

    int minIX, minIY, minIZ;
    int maxIX, maxIY, maxIZ;

    GetCellIndices(minBound.x, minBound.y, minBound.z, minIX, minIY, minIZ);
    GetCellIndices(maxBound.x, maxBound.y, maxBound.z, maxIX, maxIY, maxIZ);

    for (int ix = minIX; ix <= maxIX; ++ix)
    {
        if (ix < 0 || ix >= m_divisionsX) continue;
        for (int iy = minIY; iy <= maxIY; ++iy)
        {
            if (iy < 0 || iy >= m_divisionsY) continue;
            for (int iz = minIZ; iz <= maxIZ; ++iz)
            {
                if (iz < 0 || iz >= m_divisionsZ) continue;
                Cell& cell = m_grid[ix][iy][iz];
                for (size_t i = 0; i < cell.objects.size(); ++i)
                {
                    if (cell.objects[i] == pObject)
                    {
                        cell.objects.erase(cell.objects.begin() + i);
                        i--;
                    }
                }
            }
        }
    }
}

std::vector<sObject*> CSpatialPartition::QueryRay(float startX, float startY, float startZ, float endX, float endY, float endZ)
{
    std::set<sObject*> uniqueResults;

    GGVECTOR3 rayStart(startX, startY, startZ);
    GGVECTOR3 rayDir(endX - startX, endY - startY, endZ - startZ);

    float rayLength = GGVec3Length(&rayDir);
    if (rayLength < 0.0001f) { // Check for zero-length ray early
        std::vector<sObject*> emptyVec;
        return emptyVec;
    }
    GGVec3Normalize(&rayDir, &rayDir);

    int ix, iy, iz;
    GetCellIndices(startX, startY, startZ, ix, iy, iz);

    // Check if ray starts outside grid (and handle if necessary, for now assume it starts within or GetCellIndices clamps it)
    // A more robust check:
    bool startsOutside = (startX < m_origin.x || startX >= m_origin.x + m_worldSizeX ||
                          startY < m_origin.y || startY >= m_origin.y + m_worldSizeY ||
                          startZ < m_origin.z || startZ >= m_origin.z + m_worldSizeZ);

    // If starting outside and not pointing towards the grid, can't hit. (Simplified check)
    if(startsOutside) {
         // A proper ray-AABB intersection with the whole grid would be needed here to find entry point.
         // For now, if starts outside and clamped indices are at border and ray points away, exit.
         if ((ix == 0 && rayDir.x < 0) || (ix == m_divisionsX - 1 && rayDir.x > 0) ||
             (iy == 0 && rayDir.y < 0) || (iy == m_divisionsY - 1 && rayDir.y > 0) ||
             (iz == 0 && rayDir.z < 0) || (iz == m_divisionsZ - 1 && rayDir.z > 0)) {
             // This check is not perfect but tries to avoid processing rays clearly missing the grid.
         }
    }


    int stepX = (rayDir.x > 0.0f) ? 1 : ((rayDir.x < 0.0f) ? -1 : 0);
    int stepY = (rayDir.y > 0.0f) ? 1 : ((rayDir.y < 0.0f) ? -1 : 0);
    int stepZ = (rayDir.z > 0.0f) ? 1 : ((rayDir.z < 0.0f) ? -1 : 0);

    float tMaxX = FLT_MAX, tMaxY = FLT_MAX, tMaxZ = FLT_MAX;
    float tDeltaX = FLT_MAX, tDeltaY = FLT_MAX, tDeltaZ = FLT_MAX;

    if (stepX != 0) {
        float nextBoundaryX = m_origin.x + (ix + (stepX > 0 ? 1 : 0)) * m_cellSizeX;
        tMaxX = (nextBoundaryX - startX) / rayDir.x; // rayDir.x cannot be 0 if stepX != 0
        tDeltaX = m_cellSizeX / std::fabs(rayDir.x);
    }
    if (stepY != 0) {
        float nextBoundaryY = m_origin.y + (iy + (stepY > 0 ? 1 : 0)) * m_cellSizeY;
        tMaxY = (nextBoundaryY - startY) / rayDir.y; // rayDir.y cannot be 0 if stepY != 0
        tDeltaY = m_cellSizeY / std::fabs(rayDir.y);
    }
    if (stepZ != 0) {
        float nextBoundaryZ = m_origin.z + (iz + (stepZ > 0 ? 1 : 0)) * m_cellSizeZ;
        tMaxZ = (nextBoundaryZ - startZ) / rayDir.z; // rayDir.z cannot be 0 if stepZ != 0
        tDeltaZ = m_cellSizeZ / std::fabs(rayDir.z);
    }

    float currentT = 0.0f; // Distance travelled along the ray

    for (;;) // Loop indefinitely until break
    {
        if (ix >= 0 && ix < m_divisionsX && iy >= 0 && iy < m_divisionsY && iz >= 0 && iz < m_divisionsZ)
        {
            Cell& cell = m_grid[ix][iy][iz];
            for (sObject* pObj : cell.objects) {
                uniqueResults.insert(pObj);
            }
        } else {
            // Should not happen if ray starts inside and step logic is correct,
            // but as a safeguard if GetCellIndices somehow returns out of bounds initially.
            break;
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                currentT = tMaxX;
                ix += stepX;
                tMaxX += tDeltaX;
            } else {
                currentT = tMaxZ;
                iz += stepZ;
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                currentT = tMaxY;
                iy += stepY;
                tMaxY += tDeltaY;
            } else {
                currentT = tMaxZ;
                iz += stepZ;
                tMaxZ += tDeltaZ;
            }
        }

        if (currentT > rayLength) break; // Ray has exited the segment of interest

        if (ix < 0 || ix >= m_divisionsX || iy < 0 || iy >= m_divisionsY || iz < 0 || iz >= m_divisionsZ) {
            break; // Ray has exited the grid
        }

        // Safety break for rays parallel to an axis and starting on a grid line, though tMax should be FLT_MAX.
        if (stepX == 0 && stepY == 0 && stepZ == 0) break;
    }

    std::vector<sObject*> resultVec;
    resultVec.reserve(uniqueResults.size());
    for(sObject* pObj : uniqueResults) {
        resultVec.push_back(pObj);
    }
    return resultVec;
}
