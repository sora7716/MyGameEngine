#include "Wireframe.hlsli"

struct TransformationMatrix {
    float4x4 world;
};

//Camera
struct Camera {
    float3 worldPosition;
    float padding;
    float4x4 viewProjection;
};

ConstantBuffer<TransformationMatrix> gTrasformationMatrix : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);

struct VertexShaderInput {
    float4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float4x4 worldViewProjection = mul(gTrasformationMatrix.world, gCamera.viewProjection);
    output.position = mul(input.position, worldViewProjection);
    return output;
}