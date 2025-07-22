#include "stdafx.h"
#include "gameguru.h"
#include "PortalRenderer.h"
#include "CCameraC.h"

PortalRenderer::PortalRenderer()
{
}

PortalRenderer::~PortalRenderer()
{
}

void PortalRenderer::addPortal(const cVector3& p1, const cVector3& p2, const cVector3& p3, const cVector3& p4)
{
    Portal portal;
    portal.points[0] = p1;
    portal.points[1] = p2;
    portal.points[2] = p3;
    portal.points[3] = p4;
    portals.push_back(portal);
}

void PortalRenderer::render()
{
    for (auto& portal : portals)
    {
        if (isPortalVisible(portal))
        {
            if (!portal.pTexture)
            {
                createPortalRenderTarget(portal);
            }

            // Set the render target to the portal's render target.
            LPGGSURFACE pOldRenderTarget;
            m_pD3D->GetRenderTarget(0, &pOldRenderTarget);
            m_pD3D->SetRenderTarget(0, portal.pSurface);

            // Create a new view and projection matrix for the portal.
            GGMATRIX viewMatrix = GetCameraViewMatrix(0);
            GGMATRIX projMatrix = GetCameraProjectionMatrix(0);
            GGMATRIX portalViewMatrix;
            GGMATRIX portalProjMatrix;
            cVector3 portalCenter = (portal.points[0] + portal.points[1] + portal.points[2] + portal.points[3]) / 4.0f;
            cVector3 portalNormal = (portal.points[1] - portal.points[0]).Cross(portal.points[2] - portal.points[0]);
            portalNormal.Normalize();
            GGMatrixLookAtLH(&portalViewMatrix, &portalCenter, &(portalCenter + portalNormal), &cVector3(0, 1, 0));
            GGMatrixPerspectiveFovLH(&portalProjMatrix, GGToRadian(90.0f), 1.0f, 0.1f, 1000.0f);

            // Render the scene using the new matrices.
            SetViewMatrix(0, &portalViewMatrix);
            SetProjectionMatrix(0, &portalProjMatrix);
            // This is where the scene would be rendered.

            // Restore the original render target and matrices.
            m_pD3D->SetRenderTarget(0, pOldRenderTarget);
            SAFE_RELEASE(pOldRenderTarget);
            SetViewMatrix(0, &viewMatrix);
            SetProjectionMatrix(0, &projMatrix);

            // Render the portal to the screen.
            int objectID = 1000000 + (int)(&portal - &portals[0]);
            if (!ObjectExist(objectID))
            {
                MakeObjectPlane(objectID, 1, 1);
            }
            PositionObject(objectID, portal.points[0].x, portal.points[0].y, portal.points[0].z);
            RotateObject(objectID, 0, 0, 0);
            ScaleObject(objectID, 100, 100, 100);
            TextureObject(objectID, portal.pTexture);
        }
    }
}

void ExtractFrustum(GGPLANE* pFrustum, const GGMATRIX& viewProj)
{
    // Left plane
    pFrustum[0].a = viewProj._14 + viewProj._11;
    pFrustum[0].b = viewProj._24 + viewProj._21;
    pFrustum[0].c = viewProj._34 + viewProj._31;
    pFrustum[0].d = viewProj._44 + viewProj._41;

    // Right plane
    pFrustum[1].a = viewProj._14 - viewProj._11;
    pFrustum[1].b = viewProj._24 - viewProj._21;
    pFrustum[1].c = viewProj._34 - viewProj._31;
    pFrustum[1].d = viewProj._44 - viewProj._41;

    // Top plane
    pFrustum[2].a = viewProj._14 - viewProj._12;
    pFrustum[2].b = viewProj._24 - viewProj._22;
    pFrustum[2].c = viewProj._34 - viewProj._32;
    pFrustum[2].d = viewProj._44 - viewProj._42;

    // Bottom plane
    pFrustum[3].a = viewProj._14 + viewProj._12;
    pFrustum[3].b = viewProj._24 + viewProj._22;
    pFrustum[3].c = viewProj._34 + viewProj._32;
    pFrustum[3].d = viewProj._44 + viewProj._42;

    // Near plane
    pFrustum[4].a = viewProj._13;
    pFrustum[4].b = viewProj._23;
    pFrustum[4].c = viewProj._33;
    pFrustum[4].d = viewProj._43;

    // Far plane
    pFrustum[5].a = viewProj._14 - viewProj._13;
    pFrustum[5].b = viewProj._24 - viewProj._23;
    pFrustum[5].c = viewProj._34 - viewProj._33;
    pFrustum[5].d = viewProj._44 - viewProj._43;

    // Normalize the planes
    for (int i = 0; i < 6; ++i)
    {
        float length = sqrt(pFrustum[i].a * pFrustum[i].a + pFrustum[i].b * pFrustum[i].b + pFrustum[i].c * pFrustum[i].c);
        pFrustum[i].a /= length;
        pFrustum[i].b /= length;
        pFrustum[i].c /= length;
        pFrustum[i].d /= length;
    }
}

bool PortalRenderer::isPortalVisible(const Portal& portal)
{
    // Get the camera's view-projection matrix.
    GGMATRIX viewMatrix = GetCameraViewMatrix(0);
    GGMATRIX projMatrix = GetCameraProjectionMatrix(0);
    GGMATRIX viewProjMatrix = viewMatrix * projMatrix;

    // Extract the view frustum planes.
    GGPLANE frustum[6];
    ExtractFrustum(frustum, viewProjMatrix);

    // Create a bounding box for the portal.
    cVector3 min = portal.points[0];
    cVector3 max = portal.points[0];
    for (int i = 1; i < 4; ++i)
    {
        if (portal.points[i].x < min.x) min.x = portal.points[i].x;
        if (portal.points[i].y < min.y) min.y = portal.points[i].y;
        if (portal.points[i].z < min.z) min.z = portal.points[i].z;
        if (portal.points[i].x > max.x) max.x = portal.points[i].x;
        if (portal.points[i].y > max.y) max.y = portal.points[i].y;
        if (portal.points[i].z > max.z) max.z = portal.points[i].z;
    }

    // Check if the bounding box intersects with the view frustum.
    for (int i = 0; i < 6; ++i)
    {
        if (GGPlaneDotCoord(&frustum[i], &cVector3(min.x, min.y, min.z)) > 0) continue;
        if (GGPlaneDotCoord(&frustum[i], &cVector3(max.x, min.y, min.z)) > 0) continue;
        if (GGPlaneDotCoord(&frustum[i], &cVector3(min.x, max.y, min.z)) > 0) continue;
        if (GGPlaneDotCoord(&frustum[i], &cVector3(max.x, max.y, min.z)) > 0) continue;
        if (GGPlaneDotCoord(&frustum[i], &cVector3(min.x, min.y, max.z)) > 0) continue;
        if (GGPlaneDotCoord(&frustum[i], &cVector3(max.x, min.y, max.z)) > 0) continue;
        if (GGPlaneDotCoord(&frustum[i], &cVector3(min.x, max.y, max.z)) > 0) continue;
        if (GGPlaneDotCoord(&frustum[i], &cVector3(max.x, max.y, max.z)) > 0) continue;
        return false;
    }

    return true;
}

void PortalRenderer::createPortalRenderTarget(Portal& portal)
{
    // Create a new render target for the portal.
    int width = 512;
    int height = 512;
    portal.pTexture = MakeImageRenderTarget(0, width, height, GGFMT_A8R8G8B8);
    portal.pTexture->GetSurfaceLevel(0, &portal.pSurface);
}
