#pragma once

#include <vector>
#include "cVectorC.h"

class PortalRenderer
{
public:
    PortalRenderer();
    ~PortalRenderer();

    void addPortal(const cVector3& p1, const cVector3& p2, const cVector3& p3, const cVector3& p4);
    void render();

private:
    struct Portal
    {
        cVector3 points[4];
        LPGGTEXTURE pTexture;
        LPGGSURFACE pSurface;
    };

    bool isPortalVisible(const Portal& portal);
    void createPortalRenderTarget(Portal& portal);

    std::vector<Portal> portals;
};
