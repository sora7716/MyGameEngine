#include "Shape.hlsli"

struct TransformationMatrix {
    float32_t4x4 wvp;
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
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float32_t4x4 worldViewProjection = mul(gTrasformationMatrix.world, gCamera.viewProjection);
    output.position = mul(input.position, worldViewProjection);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTrasformationMatrix.world));
    output.worldPosition = mul(input.position, gTrasformationMatrix.world).xyz;
    return output;
}