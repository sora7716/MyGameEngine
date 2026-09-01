#include "Particle.hlsli"

struct ParticleForGPU {
    float32_t4x4 world;
    float32_t4 color;
};
//Camera
struct Camera {
    float32_t3 worldPosition;
    float32_t padding;
    float32_t4x4 viewProjection;
};

StructuredBuffer<ParticleForGPU> gParticle : register(t0);
ConstantBuffer<Camera> gCamera : register(b0);

struct VertexShaderInput {
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {
    VertexShaderOutput output;
    float32_t4x4 worldViewProjection = mul(gParticle[instanceId].world, gCamera.viewProjection);
    output.position = mul(input.position, worldViewProjection);
    output.texcoord = input.texcoord;
    output.color = gParticle[instanceId].color;
    return output;
}