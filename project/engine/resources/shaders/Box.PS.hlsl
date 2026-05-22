#include "Box.hlsli"

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

//カメラ
struct Camera {
    float32_t3 worldPosition;
    float32_t padding;
};

//コンスタントバッファ
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);

//テクスチャ2D
Texture2D<float4> gTexture : register(t0);

//サンプラー
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

//ライティング
struct Lighting {
    float32_t3 diffuse;
    float32_t3 specular;
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
    result.specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;

    return result.diffuse + result.specular;
    //return result.diffuse;
}

PixelShaderOutput main(VertexShaderOutput input) {
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvMatrix);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    
    //Lightingする場合
    if (gMaterial.enableLighring) {
        //カメラの視点を取得
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
        //ライティングをRGBに適応
        output.color.rgb = DirectionalLighting(input, textureColor, toEye);
      
        //アルファ  
        output.color.a = gMaterial.color.a * textureColor.a;

    } else {
        output.color = gMaterial.color * textureColor;
    }
    //textureのα値が0.5f以下の時にPixelを棄却
    if (textureColor.a <= 0.5) {
        discard;
    }
    //textureのα値の0の時にPixelを棄却
    if (textureColor.a == 0.0) {
        discard;
    }
    //output.colorのα値が0の時にPixelを棄却
    if (output.color.a == 0.0) {
        discard;
    }
    return output;
}