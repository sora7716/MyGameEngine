#include "Lighting.hlsli"

//マテリアル
struct Material {
    float4 color;
    int enableLighring;
    float4x4 uvMatrix;
    float shininess;
    float environmentCoefficient;
};

//リムライト
struct RimLight {
    float4 color; //ライトの色
    float power; //リムライトの強さ
    float outLinePower; //リムライトの外側の強さ
    float softness; //リムライトの柔らかさ
    int enableRimLighting; //リムライトを有効にするか
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
TextureCube<float4> gEnvironmentTexture : register(t3);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<RimLight> gRimLight : register(b3);
StructuredBuffer<PointLight> gPointLight : register(t1);
StructuredBuffer<SpotLight> gSpotLight : register(t2);

//ピクセルシェーダで出す値
struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

//ライティングのモードを切り替える用の関数
float3 SetLightingMode(VertexShaderOutput input, float4 textureColor, float3 toEye) {
    //ベースの色
    float4 baseColor = gMaterial.color * textureColor;
    //平行光源
    float3 directionalLighting = CalculateDirectionalLight(input, baseColor, gDirectionalLight, gMaterial.shininess, toEye);
        
    //点光源
    uint pointLightCount, stridePoint;
    gPointLight.GetDimensions(pointLightCount, stridePoint);
    float3 pointLighting = { 0.0f, 0.0f, 0.0f };
    [loop]
    for (uint i = 0; i < pointLightCount; i++) {
        //点光源がそもそも有効じゃなければ
        if (!gPointLight[i].enabled) {
            continue;
        }
        pointLighting += CalculatePointLight(input, baseColor, gPointLight[i], gMaterial.shininess, toEye);
    }
    
    //スポットライト
    uint spotLightCount, strideSpot;
    gSpotLight.GetDimensions(spotLightCount, strideSpot);
    float3 spotLight = { 0.0f, 0.0f, 0.0f };
    [loop]
    for (uint i = 0; i < spotLightCount; i++) {
        if (!gSpotLight[i].enabled) {
            continue;
        }
        spotLight += CalculateSpotLight(input, baseColor, gSpotLight[i], gMaterial.shininess, toEye);
    }
        
    float3 directionalRgb = {
        0.0f, 0.0f, 0.0f
    };
    
    if (gDirectionalLight.enabled) {
        directionalRgb = directionalLighting;
    }
    
    return directionalRgb + pointLighting + spotLight;
}

//リムライト
float4 RimLighting(VertexShaderOutput input, float3 toEye) {
    //リムライト
    float4 rimColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (!gRimLight.enableRimLighting) {
        return rimColor;
    }
    
    //カメラ基準
    float rim = 1.0f - saturate(dot(toEye, input.normal));
    
    //合成
    float rimValue = pow(rim, gRimLight.power);
    rimValue = smoothstep(gRimLight.outLinePower - gRimLight.softness, gRimLight.outLinePower + gRimLight.softness, rimValue);
    rimColor = gRimLight.color * rimValue;
    
    return rimColor;
}

PixelShaderOutput main(VertexShaderOutput input) {
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvMatrix);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    
    //Lightingする場合
    if (gMaterial.enableLighring) {
        //カメラの視点を取得
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
        //リムライト
        float4 rimColor = RimLighting(input, toEye);
        
        //ライティングをRGBに適応
        output.color.rgb = SetLightingMode(input, textureColor, toEye);
        
        //リムライトを加算
        output.color.rgb += rimColor.rgb;
        
        //環境マップ
        float3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
        float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
        output.color.rgb += environmentColor.rgb * gMaterial.environmentCoefficient;
      
        //アルファ  
        output.color.a = gMaterial.color.a * textureColor.a;

    } else {
        output.color = gMaterial.color * textureColor;
    }
    
    // テクスチャのα値をが一定数以上下回ったら
    if (textureColor.a <= 0.5f) {
        discard;
    }
    return output;
}