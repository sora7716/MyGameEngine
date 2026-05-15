#include "Box.hlsli"

//マテリアル
struct Material {
    float32_t4 color;
    int32_t enableLighring;
    float32_t4x4 uvMatrix;
    float32_t shininess;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvMatrix);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    output.color = gMaterial.color * textureColor;
    ////Lightingする場合
    //if (gMaterial.enableLighring) {
    //    //カメラの視点を取得
    //    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
    //    //リムライト
    //    float32_t4 rimColor = RimLighting(input, toEye);
        
    //    //ライティングをRGBに適応
    //    output.color.rgb = SetLightingMode(input, textureColor, toEye);
        
    //    //リムライトを加算
    //    output.color.rgb += rimColor.rgb;
      
    //    //アルファ  
    //    output.color.a = gMaterial.color.a * textureColor.a;

    //} else {
    //    output.color = gMaterial.color * textureColor;
    //}
    //textureのα値が0.5f以下の時にPixelを棄却
    if (textureColor.a <= 0.5f) {
        discard;
    }
    //textureのα値の0の時にPixelを棄却
    if (textureColor.a == 0.0f) {
        discard;
    }
    //output.colorのα値が0の時にPixelを棄却
    if (output.color.a == 0.0f) {
        discard;
    }
    return output;
}