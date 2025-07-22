// Voxel-based global illumination shader

Texture3D<float4> VoxelTexture : register(t0);
SamplerState VoxelSampler : register(s0);

cbuffer VoxelGI
{
    float3 g_vLightPosition;
    float3 g_vLightColor;
    float3 g_vCameraPosition;
};

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD0) : SV_TARGET
{
    // Get the ray direction.
    float3 rayDirection = normalize(float3(texcoord.x * 2 - 1, (1 - texcoord.y) * 2 - 1, 1));

    // Trace the ray through the voxel grid.
    float3 currentPosition = g_vCameraPosition;
    for (int i = 0; i < 128; ++i)
    {
        // Check if the current position is inside a voxel.
        if (VoxelTexture.Sample(VoxelSampler, currentPosition).a > 0)
        {
            // If the current position is inside a voxel, calculate the lighting at that point.
            float3 lighting = float3(0, 0, 0);

            // Cast a ray from the current position to the light source.
            float3 lightDirection = normalize(g_vLightPosition - currentPosition);
            float3 lightRayPosition = currentPosition;
            bool occluded = false;
            for (int k = 0; k < 128; ++k)
            {
                // Check if the light ray is occluded by another voxel.
                if (VoxelTexture.Sample(VoxelSampler, lightRayPosition).a > 0)
                {
                    occluded = true;
                    break;
                }
                lightRayPosition += lightDirection;
            }

            // If the light ray is not occluded, add the light source's contribution to the lighting.
            if (!occluded)
            {
                lighting += g_vLightColor;
            }

            // Return the final color.
            return float4(lighting, 1);
        }

        // Move to the next position along the ray.
        currentPosition += rayDirection;
    }

    // If the ray does not hit a voxel, return black.
    return float4(0, 0, 0, 1);
}
