#include "SkyBox.hlsli"

struct TransformationMatrix {
    float32_t4x4 WVP;
    float32_t4x4 world;
};

//Camera
struct Camera {
    float32_t3 worldPosition;
    float32_t padding;
    float32_t4x4 viewProjection;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);

struct VertexShaderInput {
    float32_t4 position : POSITION0;
    float32_t3 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    float32_t4x4 worldViewProjection = mul(gTransformationMatrix.world, gCamera.viewProjection);
    output.position = mul(input.position, worldViewProjection).xyww;
    output.texcoord = input.texcoord.xyz;
    return output;
}