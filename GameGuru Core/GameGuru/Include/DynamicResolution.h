#pragma once

class DynamicResolution
{
public:
    DynamicResolution();
    ~DynamicResolution();

    void setResolution(int width, int height);
    void setSlice(int slice, float near, float far);
    void apply();

private:
    RECT calculateViewport(int slice);

    int width;
    int height;
    int slice;
    float near;
    float far;
};
