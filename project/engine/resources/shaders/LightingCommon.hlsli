//頂点の戻り値
struct VertexShaderOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
};

//Camera
struct Camera {
    float3 worldPosition;
    float padding;
    float4x4 viewProjection;
};