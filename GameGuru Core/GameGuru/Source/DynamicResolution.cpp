#include "stdafx.h"
#include "gameguru.h"
#include "DynamicResolution.h"

DynamicResolution::DynamicResolution()
{
    width = 0;
    height = 0;
    slice = 0;
    near = 0.0f;
    far = 0.0f;
}

DynamicResolution::~DynamicResolution()
{
}

void DynamicResolution::setResolution(int width, int height)
{
    this->width = width;
    this->height = height;
}

void DynamicResolution::setSlice(int slice, float near, float far)
{
    this->slice = slice;
    this->near = near;
    this->far = far;
}

void DynamicResolution::apply()
{
    // Get the current viewport.
    RECT oldViewport;
    GetViewport(&oldViewport.left, &oldViewport.top, &oldViewport.right, &oldViewport.bottom);

    // Calculate the new viewport for the current slice.
    RECT newViewport = calculateViewport(slice);

    // Set the new viewport.
    SetViewport(newViewport.left, newViewport.top, newViewport.right, newViewport.bottom);

    // Create a new projection matrix for the current slice.
    GGMATRIX projectionMatrix;
    GGMatrixPerspectiveFovLH(&projectionMatrix, GGToRadian(75.0f), (float)newViewport.right / (float)newViewport.bottom, near, far);

    // Set the new projection matrix.
    SetProjectionMatrix(0, &projectionMatrix);
}

RECT DynamicResolution::calculateViewport(int slice)
{
    RECT viewport;
    if (slice == 0)
    {
        viewport.left = 0;
        viewport.top = 0;
        viewport.right = width;
        viewport.bottom = height;
    }
    else
    {
        viewport.left = 0;
        viewport.top = 0;
        viewport.right = width / 2;
        viewport.bottom = height / 2;
    }
    return viewport;
}
