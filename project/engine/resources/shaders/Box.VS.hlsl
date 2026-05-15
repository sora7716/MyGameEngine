#include "Box.hlsli"

struct TransformationMatrix {
    float32_t4x4 wvp;
    float32_t4x4 world;
};
ConstantBuffer<TransformationMatrix> gTrasformationMatrix : register(b0);

struct VertexShaderInput {
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTrasformationMatrix.wvp);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTrasformationMatrix.world));
    output.worldPosition = mul(input.position, gTrasformationMatrix.world).xyz;
    return output;
}