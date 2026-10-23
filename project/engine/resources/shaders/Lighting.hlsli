#include "LightingCommon.hlsli"

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

//ライティング
struct Lighting {
    float3 baseColor;
    float3 lightColor;
    float3 diffuse;
    float3 specular;
};

//拡散反射光の明るさの係数の計算
float CalculateDiffuseFactor(float3 normal, float3 direction, int isLambert) {
    float result = 0.0f;
    if (isLambert) {
        //lambert
        result = saturate(dot(normalize(normal), -direction));
    } else {
        //half lambert
        float NdotL = dot(normalize(normal), -direction);
        //拡散反射
        result = pow(NdotL * 0.5f + 0.5f, 2.0f);
    }
    
    return result;
}

//鏡面反射の強さを計算
float CalculateSpecularFactor(float3 normal, float3 toEye, float shininess, float3 direction, int isBlinnPhong) {
    float result = 0.0f;
    if (isBlinnPhong) {
        //BlingPhongReflectionModel
        //ハーフベクトル
        float3 halfVector = normalize(-direction + toEye);
        float NDotH = dot(normalize(normal), halfVector);
        result = pow(saturate(NDotH), shininess); //反射強度
    } else {
        //PhongReflectionModel
        //入射光の反射ベクトルを求める
        float3 reflectLight = reflect(direction, normalize(normal));
        //鏡面反射の強度
        float RdotE = dot(reflectLight, toEye);
        result = pow(saturate(RdotE), shininess); //反射強度
    }
    
    return result;
}

//距離減衰の計算
float CalculateDistanceAttenuation(float3 toLight,float distance,float decay) {
    //実際の距離
    float actualDistance = length(toLight);
    //距離減衰(1 - 実際の距離/最大距離)
    float attenuation = saturate(1.0f - (actualDistance / distance));
    //最終的な減衰を計算
    attenuation = pow(attenuation,decay);
    
    return attenuation;
}

//平行光源
float3 CalculateDirectionalLight(VertexShaderOutput input, float4 baseColor, DirectionalLight directionalLight, float shininess, float3 toEye) {
     //ライティング
    Lighting result;
    
    //拡散反射光の明るさ
    float diffuseFactor = CalculateDiffuseFactor(input.normal, directionalLight.direction, directionalLight.isLambert);
        
    //鏡面反射の強度
    float specularFactor = CalculateSpecularFactor(input.normal, toEye, shininess, directionalLight.direction, directionalLight.isBlinnPhong);
    
    //元の色
    result.baseColor = baseColor.rgb;
    //光の色と強さ
    result.lightColor = directionalLight.color.rgb * directionalLight.intensity;
    //拡散反射光
    result.diffuse = result.baseColor * diffuseFactor;
    //鏡面反射
    result.specular = specularFactor * float3(1.0f, 1.0f, 1.0f);
    return (result.diffuse + result.specular) * result.lightColor;
}

//点光源
float3 CalculatePointLight(VertexShaderOutput input, float4 baseColor, PointLight pointLight, float shininess, float3 toEye) {
    //ライティング
    Lighting result;
    
    //ライトまでの距離
    float3 toLight = input.worldPosition - pointLight.position;
    //点光源の方向ベクトル
    float3 pointLightDirection = normalize(toLight);
   
    //拡散反射光の明るさ
    float diffuseFactor = CalculateDiffuseFactor(input.normal, pointLightDirection, pointLight.isLambert);
    
    //鏡面反射の強度
    float specularFactor = CalculateSpecularFactor(input.normal, toEye, shininess, pointLightDirection, pointLight.isBlinnPhong);
    
    //距離減衰(1 - 実際の距離/最大距離)
    float attenuation = CalculateDistanceAttenuation(toLight, pointLight.distance, pointLight.decay);
    
    //元の色
    result.baseColor = baseColor.rgb;
    //光の色
    result.lightColor = pointLight.color.rgb * pointLight.intensity;
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
float3 CalculateSpotLight(VertexShaderOutput input, float4 baseColor, SpotLight spotLight, float shininess, float3 toEye) {
    //ライティング
    Lighting result;
    
    //ライトの位置
    float3 toLight = input.worldPosition - spotLight.position;
    
    //スポットライトの入射光
    float3 spotLightDirectionOnSurface = normalize(toLight);
    
    //コサイン
    float diffuseFactor = CalculateDiffuseFactor(input.normal, spotLightDirectionOnSurface, spotLight.isLambert);

     //鏡面反射の強度
    float specularFactor = CalculateSpecularFactor(input.normal, toEye, shininess, spotLightDirectionOnSurface, spotLight.isBlinnPhong);
        
    //元の色
    result.baseColor = baseColor.rgb;
    //ライトの色
    result.lightColor = spotLight.color.rgb * spotLight.intensity;
    //拡散反射
    result.diffuse = result.baseColor * diffuseFactor;
    //鏡面反射
    result.specular = specularFactor * float3(1.0f, 1.0f, 1.0f);
    
    //光の円の計算
    //外接円
    float cosAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
    //内接円
    float falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));
        
    //光の減衰
    float attenuation = CalculateDistanceAttenuation(toLight, spotLight.distance, spotLight.decay);
        
    //光の減衰を適応
    result.diffuse *= attenuation * falloffFactor;
    result.specular *= attenuation * falloffFactor;
    
    return (result.diffuse + result.specular) * result.lightColor;
}