
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