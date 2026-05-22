#include "Object3d.hlsli"

//マテリアル
struct Material {
    float32_t4 color;
    int32_t enableLighring;
    float32_t4x4 uvMatrix;
    float32_t shininess;
};

//平行光源
struct DirectionalLight {
    float32_t4 color; //ライトの色
    float32_t3 direction; //ライトの向き
    float32_t intensity; //輝度
    int32_t isLambert; //lambertにするかどうか
    int32_t isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int32_t enableDirectonalLighting; //平行光源を有効にするか
};

//点光源
struct PointLight {
    float32_t4 color; //ライトの色
    float32_t3 position; //ライトの位置
    float32_t intensity; //輝度
    float32_t distance; //ライトが届く最大距離
    float32_t decay; //減衰率
    int32_t isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int32_t enablePointLighting; //点光源を有効にするか
};

//スポットライト
struct SpotLight {
    float32_t4 color; //ライトの色
    float32_t3 position; //ライトの位置
    float32_t intensity; //輝度
    float32_t3 direction; //スポットライトの方向
    float32_t distance; //ライトの届く最大距離
    float32_t decay; //減衰率
    float32_t cosAngle; //スポットライトの余弦
    float32_t cosFalloffStart;
    int32_t isBlinnPhong; //BlinnPhongReflectionを行うかどうか
    int32_t enablSpotLighting; //点光源を有効にするか
    float32_t2 padding;
};

//リムライト
struct RimLight {
    float32_t4 color; //ライトの色
    float32_t power; //リムライトの強さ
    float32_t outLinePower; //リムライトの外側の強さ
    float32_t softness; //リムライトの柔らかさ
    int32_t enableRimLighting; //リムライトを有効にするか
};

//カメラ
struct Camera {
    float32_t3 worldPosition;
    float32_t padding;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<RimLight> gRimLight : register(b3);
StructuredBuffer<PointLight> gPointLight : register(t1);
StructuredBuffer<SpotLight> gSpotLight : register(t2);

//ライティング
struct Lighting {
    float32_t3 diffuse;
    float32_t3 specular;
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

//平行光源
float32_t3 DirectionalLighting(VertexShaderOutput input, float32_t4 textureColor, float32_t3 toEye) {
     //ライティング
    Lighting result;
    
    //コサイン
    float32_t NDotDirectional = 0.0f;
        
    //ライティングの方法
    if (gDirectionalLight.isLambert) {
        //lambert
        NDotDirectional = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
    } else {
        //half lambert
        float32_t NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        //拡散反射
        NDotDirectional = pow(NdotL * 0.5f + 0.5f, 2.0f);
    }
        
    //鏡面反射の強度
    float32_t specularPow = 0.0f;
    if (gDirectionalLight.isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float32_t NDotH = dot(normalize(input.normal), halfVector);
        specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        //鏡面反射の強度
        float32_t RdotE = dot(reflectLight, toEye);
        specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
    }
    
    //NdotL = step(0.25f, NdotL);
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NDotDirectional * gDirectionalLight.intensity;
        
    //鏡面反射
    result.specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);

    return result.diffuse + result.specular;
}

//点光源
float32_t3 PointLighing(VertexShaderOutput input, uint32_t instanceId, float32_t4 textureColor, float32_t3 toEye) {
    //ライティング
    Lighting result;
    
    //点光源の方向ベクトル
    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight[instanceId].position);
   
    //コサイン
    float32_t NDotPoint = saturate(dot(normalize(input.normal), -pointLightDirection));
    
    //鏡面反射の強度
    float32_t specularPow = 0.0f;
    if (gPointLight[instanceId].isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float32_t3 halfVector = normalize(-pointLightDirection + toEye);
        float32_t NDotH = dot(normalize(input.normal), halfVector);
        //反射強度
        specularPow = pow(saturate(NDotH), gMaterial.shininess);
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float32_t3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
        //鏡面反射の強度
        float32_t RdotE = dot(reflectLight, toEye);
        //反射強度
        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }
    
    //距離減衰
    float32_t attenuation = 1.0f / (gPointLight[instanceId].distance * gPointLight[instanceId].decay);
    
    //拡散反射
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gPointLight[instanceId].color.rgb * NDotPoint * gPointLight[instanceId].intensity;
    //鏡面反射
    result.specular = gPointLight[instanceId].color.rgb * gPointLight[instanceId].intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
    
    //距離減衰を適応
    result.diffuse *= attenuation;
    result.specular *= attenuation;
    return result.diffuse + result.specular;
}

//スポットライト
float32_t3 SpotLightint(VertexShaderOutput input, uint32_t instanceId, float32_t4 textureColor, float32_t3 toEye) {
    //ライティング
    Lighting result;
    
    //スポットライトの入射光
    float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight[instanceId].position);
    
    //コサイン
    float32_t NDotSpot = saturate(dot(normalize(input.normal), -spotLightDirectionOnSurface));
        
     //鏡面反射の強度
    float specularPow = 0.0f;
    if (gSpotLight[instanceId].isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float32_t3 halfVector = normalize(-spotLightDirectionOnSurface + toEye);
        float32_t NDotH = dot(normalize(input.normal), halfVector);
        //反射強度
        specularPow = pow(saturate(NDotH), gMaterial.shininess);
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float32_t3 reflectLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
        //鏡面反射の強度
        float32_t RdotE = dot(reflectLight, toEye);
        //反射強度
        specularPow = pow(saturate(RdotE), gMaterial.shininess);
    }
        
    //拡散反射
    result.diffuse = gMaterial.color.rgb * textureColor.rgb * gSpotLight[instanceId].color.rgb * NDotSpot * gSpotLight[instanceId].intensity;

    //鏡面反射
    result.specular = gSpotLight[instanceId].color.rgb * gSpotLight[instanceId].intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
    float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLight[instanceId].direction);
    float32_t falloffFactor = saturate((cosAngle - gSpotLight[instanceId].cosAngle) / (gSpotLight[instanceId].cosFalloffStart - gSpotLight[instanceId].cosAngle));
        
    //光の減衰
    float32_t attenuation = 1.0f / (gSpotLight[instanceId].distance * gSpotLight[instanceId].decay);
        
    //光の減衰を適応
    result.diffuse *= attenuation * falloffFactor;
    result.specular *= attenuation * falloffFactor;
    
    return result.diffuse + result.specular;
}

//ライティングのモードを切り替える用の関数
float32_t3 SetLightingMode(VertexShaderOutput input, float32_t4 textureColor, float32_t3 toEye) {
    //ライティング
    //平行光源
    float32_t3 directionalLighting = DirectionalLighting(input, textureColor, toEye);
        
    //点光源
    uint32_t pointLightCount, stridePoint;
    gPointLight.GetDimensions(pointLightCount, stridePoint);
    float32_t3 pointLighting = { 0.0f, 0.0f, 0.0f };
    [loop]
    for (uint32_t i = 0; i < pointLightCount; i++) {
        if (!gPointLight[i].enablePointLighting) {
            continue;
        }
        pointLighting += PointLighing(input, i, textureColor, toEye);

    }
    
    //スポットライト
    uint32_t spotLightCount, strideSpot;
    gSpotLight.GetDimensions(spotLightCount, strideSpot);
    float32_t3 spotLight = { 0.0f, 0.0f, 0.0f };
    [loop]
    for (uint32_t j = 0; j < spotLightCount; j++) {
        if (!gSpotLight[j].enablSpotLighting) {
            continue;
        }
        spotLight += SpotLightint(input, j, textureColor, toEye);
    }
        
    float32_t3 directionalRgb = {
        0.0f, 0.0f, 0.0f
    };
    
    if (gDirectionalLight.enableDirectonalLighting) {
        directionalRgb = directionalLighting;
    }
    
    return directionalRgb + pointLighting + spotLight;
}

//リムライト
float32_t4 RimLighting(VertexShaderOutput input, float32_t3 toEye){
    //リムライト
    float32_t4 rimColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (!gRimLight.enableRimLighting){
        return rimColor;
    }
    
    //カメラ基準
    float32_t rim = 1.0f - saturate(dot(toEye,input.normal));
    
    //合成
    float32_t rimValue = pow(rim, gRimLight.power);
    rimValue = smoothstep(gRimLight.outLinePower - gRimLight.softness, gRimLight.outLinePower + gRimLight.softness, rimValue);
    rimColor = gRimLight.color * rimValue;
    
    return rimColor;
}

PixelShaderOutput main(VertexShaderOutput input) {
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvMatrix);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    
    //Lightingする場合
    if (gMaterial.enableLighring) {
        //カメラの視点を取得
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
        //リムライト
        float32_t4 rimColor = RimLighting(input, toEye);
        
        //ライティングをRGBに適応
        output.color.rgb = SetLightingMode(input, textureColor, toEye);
        
        //リムライトを加算
        output.color.rgb += rimColor.rgb;
      
        //アルファ  
        output.color.a = gMaterial.color.a * textureColor.a;

    } else {
        output.color = gMaterial.color * textureColor;
    }
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