#include "DebugDraw.hlsli"

struct TransformationMatrix {
    float32_t4x4 world;
};

//Camera
struct Camera {
    float32_t3 worldPosition;
    float32_t padding;
    float32_t4x4 viewProjection;
};

ConstantBuffer<TransformationMatrix> gTrasformationMatrix : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);

struct VertexShaderInput {
    float32_t4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float32_t4x4 worldViewProjection = mul(gTrasformationMatrix.world, gCamera.viewProjection);
    output.position = mul(input.position, worldViewProjection);
    return output;
}