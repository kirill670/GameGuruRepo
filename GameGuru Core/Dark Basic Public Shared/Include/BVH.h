#pragma once

#include <vector>
#include "cVectorC.h"

struct BVHNode
{
    cVector3 bounds[2];
    int left;
    int right;
    int objectID;
};

class BVH
{
public:
    BVH(const std::vector<int>& objectIDs);
    ~BVH();

    void build();
    bool traverse(const cVector3& rayOrigin, const cVector3& rayDir, int& objectID);

private:
    void buildRecursive(int nodeIndex, const std::vector<int>& objectIDs);
    bool intersectAABB(const cVector3& rayOrigin, const cVector3& rayDir, const cVector3 bounds[2]);

    std::vector<BVHNode> nodes;
    std::vector<int> objectIDs;
};
