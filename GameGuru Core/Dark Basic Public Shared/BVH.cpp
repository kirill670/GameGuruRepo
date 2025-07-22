#include "BVH.h"
#include "CObjectsC.h"

BVH::BVH(const std::vector<int>& objectIDs)
{
    this->objectIDs = objectIDs;
}

BVH::~BVH()
{
}

void BVH::build()
{
    if (objectIDs.empty())
        return;

    nodes.resize(objectIDs.size() * 2);
    buildRecursive(0, objectIDs);
}

bool BVH::traverse(const cVector3& rayOrigin, const cVector3& rayDir, int& objectID)
{
    if (nodes.empty())
        return false;

    int stack[64];
    int stackPtr = 0;
    stack[stackPtr++] = 0;

    while (stackPtr > 0)
    {
        int nodeIndex = stack[--stackPtr];
        const BVHNode& node = nodes[nodeIndex];

        if (intersectAABB(rayOrigin, rayDir, node.bounds))
        {
            if (node.objectID != -1)
            {
                objectID = node.objectID;
                return true;
            }

            if (node.left != -1)
                stack[stackPtr++] = node.left;
            if (node.right != -1)
                stack[stackPtr++] = node.right;
        }
    }

    return false;
}

void BVH::buildRecursive(int nodeIndex, const std::vector<int>& objectIDs)
{
    if (objectIDs.size() == 1)
    {
        nodes[nodeIndex].left = -1;
        nodes[nodeIndex].right = -1;
        nodes[nodeIndex].objectID = objectIDs[0];
        // calculate bounds for this object
        float minX, minY, minZ, maxX, maxY, maxZ;
        minX = minY = minZ = FLT_MAX;
        maxX = maxY = maxZ = -FLT_MAX;
        sObject* pObject = GetObjectData(objectIDs[0]);
        for (int i = 0; i < pObject->iMeshCount; ++i)
        {
            sMesh* pMesh = pObject->ppMeshList[i];
            for (int j = 0; j < pMesh->dwVertexCount; ++j)
            {
                sVertex* pVertex = (sVertex*)((BYTE*)pMesh->pVertexData + j * pMesh->dwFVFSize);
                if (pVertex->x < minX) minX = pVertex->x;
                if (pVertex->y < minY) minY = pVertex->y;
                if (pVertex->z < minZ) minZ = pVertex->z;
                if (pVertex->x > maxX) maxX = pVertex->x;
                if (pVertex->y > maxY) maxY = pVertex->y;
                if (pVertex->z > maxZ) maxZ = pVertex->z;
            }
        }
        nodes[nodeIndex].bounds[0] = cVector3(minX, minY, minZ);
        nodes[nodeIndex].bounds[1] = cVector3(maxX, maxY, maxZ);

        return;
    }

    // calculate bounds for all objects
    float minX, minY, minZ, maxX, maxY, maxZ;
    minX = minY = minZ = FLT_MAX;
    maxX = maxY = maxZ = -FLT_MAX;
    for (int objectID : objectIDs)
    {
		sObject* pObject = GetObjectData(objectID);
		for (int i = 0; i < pObject->iMeshCount; ++i)
		{
			sMesh* pMesh = pObject->ppMeshList[i];
			for (int j = 0; j < pMesh->dwVertexCount; ++j)
			{
				sVertex* pVertex = (sVertex*)((BYTE*)pMesh->pVertexData + j * pMesh->dwFVFSize);
				if (pVertex->x < minX) minX = pVertex->x;
				if (pVertex->y < minY) minY = pVertex->y;
				if (pVertex->z < minZ) minZ = pVertex->z;
				if (pVertex->x > maxX) maxX = pVertex->x;
				if (pVertex->y > maxY) maxY = pVertex->y;
				if (pVertex->z > maxZ) maxZ = pVertex->z;
			}
		}
    }
    nodes[nodeIndex].bounds[0] = cVector3(minX, minY, minZ);
    nodes[nodeIndex].bounds[1] = cVector3(maxX, maxY, maxZ);

    // split objects into two groups
    std::vector<int> leftObjects;
    std::vector<int> rightObjects;
    float split = (minX + maxX) / 2.0f;
    for (int objectID : objectIDs)
    {
        sObject* pObject = GetObjectData(objectID);
        sMesh* pMesh = pObject->ppMeshList[0];
        sVertex* pVertex = (sVertex*)pMesh->pVertexData;
        if (pVertex->x < split)
            leftObjects.push_back(objectID);
        else
            rightObjects.push_back(objectID);
    }

    if (leftObjects.empty() || rightObjects.empty())
    {
		nodes[nodeIndex].left = -1;
		nodes[nodeIndex].right = -1;
		nodes[nodeIndex].objectID = -1; // Indicates a leaf node with multiple objects
		return;
    }

    // build recursive nodes
    nodes[nodeIndex].left = nodes.size();
    nodes.emplace_back();
    buildRecursive(nodes[nodeIndex].left, leftObjects);

    nodes[nodeIndex].right = nodes.size();
    nodes.emplace_back();
    buildRecursive(nodes[nodeIndex].right, rightObjects);
}

bool BVH::intersectAABB(const cVector3& rayOrigin, const cVector3& rayDir, const cVector3 bounds[2])
{
    float tmin = -FLT_MAX;
    float tmax = FLT_MAX;

    for (int i = 0; i < 3; ++i)
    {
        if (abs(rayDir[i]) < 1e-6)
        {
            if (rayOrigin[i] < bounds[0][i] || rayOrigin[i] > bounds[1][i])
                return false;
        }
        else
        {
            float ood = 1.0f / rayDir[i];
            float t1 = (bounds[0][i] - rayOrigin[i]) * ood;
            float t2 = (bounds[1][i] - rayOrigin[i]) * ood;

            if (t1 > t2)
                std::swap(t1, t2);

            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);

            if (tmin > tmax)
                return false;
        }
    }

    return true;
}
