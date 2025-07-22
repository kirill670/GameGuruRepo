#include "stdafx.h"
#include "gameguru.h"
#include "LODManager.h"
#include "CObjectsC.h"

LODManager::LODManager()
{
}

LODManager::~LODManager()
{
}

void LODManager::addObject(int objectID)
{
    LODObject lodObject;
    lodObject.objectID = objectID;
    lodObject.lodLevel = 0;
    objects.push_back(lodObject);
}

void LODManager::update(const cVector3& cameraPosition)
{
    for (auto& lodObject : objects)
    {
        sObject* pObject = GetObjectData(lodObject.objectID);
        if (pObject)
        {
            float distance = (pObject->position.vecPosition - cameraPosition).Length();
            int newLodLevel = 0;
            if (distance > 1000.0f)
                newLodLevel = 1;
            if (distance > 2000.0f)
                newLodLevel = 2;
            if (distance > 4000.0f)
                newLodLevel = 3;

            if (newLodLevel != lodObject.lodLevel)
            {
                lodObject.lodLevel = newLodLevel;
                if (pObject->pLodTexture[newLodLevel])
                {
                    pObject->pTextures[0].pTexture = pObject->pLodTexture[newLodLevel];
                }
            }
        }
    }
}
