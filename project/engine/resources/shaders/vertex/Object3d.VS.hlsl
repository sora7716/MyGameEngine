#include "LightingCommon.hlsli"

struct TransformationMatrix{
    float4x4 world;
    float4x4 worldInverseTranspose;
};
StructuredBuffer<TransformationMatrix> gTrasformationMatrix : register(t0);
ConstantBuffer<Camera> gCamera : register(b2);

struct VertexShaderInput{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID){
    VertexShaderOutput output;
    float4x4 worldViewProjection = mul(gTrasformationMatrix[instanceId].world, gCamera.viewProjection);
    output.position = mul(input.position, worldViewProjection);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTrasformationMatrix[instanceId].worldInverseTranspose));
    output.worldPosition = mul(input.position, gTrasformationMatrix[instanceId].world).xyz;
    return output;
}