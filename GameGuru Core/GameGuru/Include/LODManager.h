#pragma once

#include <vector>
#include "cVectorC.h"

class LODManager
{
public:
    LODManager();
    ~LODManager();

    void addObject(int objectID);
    void update(const cVector3& cameraPosition);

private:
    struct LODObject
    {
        int objectID;
        int lodLevel;
    };

    std::vector<LODObject> objects;
};
