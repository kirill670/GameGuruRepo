#include "stdafx.h"
#include "gameguru.h"
#include "Voxelizer.h"
#include "CObjectsC.h"

Voxelizer::Voxelizer(int resolution)
{
    this->resolution = resolution;
    voxelData.resize(resolution * resolution * resolution);
}

Voxelizer::~Voxelizer()
{
}

void Voxelizer::voxelize(const std::vector<int>& objectIDs)
{
    for (int objectID : objectIDs)
    {
        sObject* pObject = GetObjectData(objectID);
        if (pObject)
        {
            for (int i = 0; i < pObject->iMeshCount; ++i)
            {
                sMesh* pMesh = pObject->ppMeshList[i];
                for (int j = 0; j < pMesh->dwIndexCount; j += 3)
                {
                    sVertex* pVertex1 = (sVertex*)((BYTE*)pMesh->pVertexData + pMesh->pIndices[j] * pMesh->dwFVFSize);
                    sVertex* pVertex2 = (sVertex*)((BYTE*)pMesh->pVertexData + pMesh->pIndices[j + 1] * pMesh->dwFVFSize);
                    sVertex* pVertex3 = (sVertex*)((BYTE*)pMesh->pVertexData + pMesh->pIndices[j + 2] * pMesh->dwFVFSize);
                    rasterizeTriangle(cVector3(pVertex1->x, pVertex1->y, pVertex1->z),
                                      cVector3(pVertex2->x, pVertex2->y, pVertex2->z),
                                      cVector3(pVertex3->x, pVertex3->y, pVertex3->z));
                }
            }
        }
    }
}

void Voxelizer::rasterizeTriangle(const cVector3& v0, const cVector3& v1, const cVector3& v2)
{
    // Find the bounding box of the triangle.
    cVector3 min = v0;
    cVector3 max = v0;
    min.Min(v1);
    min.Min(v2);
    max.Max(v1);
    max.Max(v2);

    // Iterate over the voxels in the bounding box.
    for (int z = (int)min.z; z <= (int)max.z; ++z)
    {
        for (int y = (int)min.y; y <= (int)max.y; ++y)
        {
            for (int x = (int)min.x; x <= (int)max.x; ++x)
            {
                // For each voxel, check if it is inside the triangle.
                cVector3 voxelCenter(x + 0.5f, y + 0.5f, z + 0.5f);
                if (isPointInTriangle(voxelCenter, v0, v1, v2))
                {
                    // If the voxel is inside the triangle, set the corresponding value in the voxel data to 1.
                    int index = x + y * resolution + z * resolution * resolution;
                    voxelData[index] = 1;
                }
            }
        }
    }
}

bool Voxelizer::isPointInTriangle(const cVector3& point, const cVector3& v0, const cVector3& v1, const cVector3& v2)
{
    // Compute vectors
    cVector3 v0v1 = v1 - v0;
    cVector3 v0v2 = v2 - v0;
    cVector3 pv0 = point - v0;

    // Compute dot products
    float dot00 = v0v1.Dot(v0v1);
    float dot01 = v0v1.Dot(v0v2);
    float dot02 = v0v1.Dot(pv0);
    float dot11 = v0v2.Dot(v0v2);
    float dot12 = v0v2.Dot(pv0);

    // Compute barycentric coordinates
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v < 1);
}
