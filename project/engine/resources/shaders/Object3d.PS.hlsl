#include "Object3d.hlsli"
#include "Lighting.hlsli"

//マテリアル
struct Material {
    float4 color;
    int enableLighring;
    float4x4 uvMatrix;
    float shininess;
    float environmentCoefficient;
};

//平行光源
struct DirectionalLight {
    float4 color; //ライトの色
    float3 direction; //ライトの向き
    float intensity; //輝度
    int isLambert; //lambertにするかどうか
    int isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int enabled; //平行光源を有効にするか
};

//点光源
struct PointLight {
    float4 color; //ライトの色
    float3 position; //ライトの位置
    float intensity; //輝度
    float distance; //ライトが届く最大距離
    float decay; //減衰率
    int isLambert; //ランバート反射
    int isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int enabled; //点光源を有効にするか
};

//スポットライト
struct SpotLight {
    float4 color; //ライトの色
    float3 position; //ライトの位置
    float intensity; //輝度
    float3 direction; //スポットライトの方向
    float distance; //ライトの届く最大距離
    float decay; //減衰率
    float cosAngle; //スポットライトの余弦
    float cosFalloffStart;
    int isLambert; //ランバート反射
    int isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int enabled; //点光源を有効にするか
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

//ライティング
struct Lighting {
    float3 baseColor;
    float3 lightColor;
    float3 diffuse;
    float3 specular;
};

//ピクセルシェーダで出す値
struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

//平行光源
float3 DirectionalLighting(VertexShaderOutput input, float4 textureColor, float3 toEye) {
     //ライティング
    Lighting result;
    
    //拡散反射光の明るさ
    float diffuseFactor = CalculateDiffuseFactor(input.normal, gDirectionalLight.direction, gDirectionalLight.isLambert);
        
    //鏡面反射の強度
    float specularFactor = CalculateSpecularFactor(input.normal, toEye, gMaterial.shininess, gDirectionalLight.direction, gDirectionalLight.isBlinnPhong);
    
    //元の色
    result.baseColor = gMaterial.color.rgb * textureColor.rgb;
    //光の色と強さ
    result.lightColor = gDirectionalLight.color.rgb * gDirectionalLight.intensity;
    //拡散反射光
    result.diffuse = result.baseColor * diffuseFactor;
    //鏡面反射
    result.specular = specularFactor * float3(1.0f, 1.0f, 1.0f);
    return (result.diffuse + result.specular) * result.lightColor;
}

//点光源
float3 PointLighing(VertexShaderOutput input, uint instanceId, float4 textureColor, float3 toEye) {
    //ライティング
    Lighting result;
    
    //ライトまでの距離
    float3 toLight = input.worldPosition - gPointLight[instanceId].position;
    //点光源の方向ベクトル
    float3 pointLightDirection = normalize(toLight);
   
    //拡散反射光の明るさ
    float diffuseFactor = CalculateDiffuseFactor(input.normal, pointLightDirection, gPointLight[instanceId].isLambert);
    
    //鏡面反射の強度
    float specularFactor = CalculateSpecularFactor(input.normal, toEye, gMaterial.shininess, pointLightDirection, gPointLight[instanceId].isBlinnPhong);
    
    //距離減衰(1 - 実際の距離/最大距離)
    float attenuation = CalculateDistanceAttenuation(toLight, gPointLight[instanceId].distance, gPointLight[instanceId].decay);
    
    //元の色
    result.baseColor = gMaterial.color.rgb * textureColor.rgb;
    //光の色
    result.lightColor = gPointLight[instanceId].color.rgb * gPointLight[instanceId].intensity;
    //拡散反射
    result.diffuse = result.baseColor * diffuseFactor;
    //鏡面反射
    result.specular = specularFactor * float3(1.0f, 1.0f, 1.0f);
    
    //距離減衰を適応
    result.diffuse *= attenuation;
    result.specular *= attenuation;
    
    return (result.diffuse + result.specular) * result.lightColor;
}

//スポットライト
float3 SpotLightint(VertexShaderOutput input, uint instanceId, float4 textureColor, float3 toEye) {
    //ライティング
    Lighting result;
    
    //ライトの位置
    float3 toLight = input.worldPosition - gSpotLight[instanceId].position;
    
    //スポットライトの入射光
    float3 spotLightDirectionOnSurface = normalize(toLight);
    
    //コサイン
    float diffuseFactor = CalculateDiffuseFactor(input.normal, spotLightDirectionOnSurface, gSpotLight[instanceId].isLambert);

     //鏡面反射の強度
    float specularFactor = CalculateSpecularFactor(input.normal, toEye, gMaterial.shininess, spotLightDirectionOnSurface, gSpotLight[instanceId].isBlinnPhong);
        
    //元の色
    result.baseColor = gMaterial.color.rgb * textureColor.rgb;
    //ライトの色
    result.lightColor = gSpotLight[instanceId].color.rgb * gSpotLight[instanceId].intensity;
    //拡散反射
    result.diffuse = result.baseColor * diffuseFactor;
    //鏡面反射
    result.specular = specularFactor * float3(1.0f, 1.0f, 1.0f);
        
    float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight[instanceId].direction);
    float falloffFactor = saturate((cosAngle - gSpotLight[instanceId].cosAngle) / (gSpotLight[instanceId].cosFalloffStart - gSpotLight[instanceId].cosAngle));
        
    //光の減衰
    float attenuation = 1.0f / (gSpotLight[instanceId].distance * gSpotLight[instanceId].decay);
        
    //光の減衰を適応
    result.diffuse *= attenuation * falloffFactor;
    result.specular *= attenuation * falloffFactor;
    
    return (result.diffuse + result.specular) * result.lightColor;
}

//ライティングのモードを切り替える用の関数
float3 SetLightingMode(VertexShaderOutput input, float4 textureColor, float3 toEye) {
    //ライティング
    //平行光源
    float3 directionalLighting = DirectionalLighting(input, textureColor, toEye);
        
    //点光源
    uint pointLightCount, stridePoint;
    gPointLight.GetDimensions(pointLightCount, stridePoint);
    float3 pointLighting = { 0.0f, 0.0f, 0.0f };
    [loop]
    for (uint i = 0; i < pointLightCount; i++) {
        if (!gPointLight[i].enabled) {
            continue;
        }
        pointLighting += PointLighing(input, i, textureColor, toEye);

    }
    
    //スポットライト
    uint spotLightCount, strideSpot;
    gSpotLight.GetDimensions(spotLightCount, strideSpot);
    float3 spotLight = { 0.0f, 0.0f, 0.0f };
    [loop]
    for (uint j = 0; j < spotLightCount; j++) {
        if (!gSpotLight[j].enabled) {
            continue;
        }
        spotLight += SpotLightint(input, j, textureColor, toEye);
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