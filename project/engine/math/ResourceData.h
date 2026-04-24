#pragma once
#include "RenderingData.h"
#include "Vector4.h"
#include <cstdint>
#include <string>
#include <vector>

//頂点データ
struct VertexData {
	Vector4 position;//直行座標
	Vector2 texcoord;//UV座標
	Vector3 normal;//法線
};

//マテリアル
struct Material {
	Vector4 color;//色
	int32_t enableLighting;//ライティングするかどうかのフラグ
	float padding[3];
	Matrix4x4 uvMatrix;//UVTransform
	float shininess;//光沢度
};

//平行光源
struct DirectionalLight {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
	int32_t isLambert;//lambertにするかどうか
	int32_t isBlinnPhong;//BlinnPhongReflectionを行うかどうか
	int32_t enableDirectionalLighting;//平行光源を有効にするか
};

//点光源
struct PointLight {
	Vector4 color;//ライトの色
	Vector3 position;//ライトの位置
	float intensity;//輝度
	float distance;//ライトの届く最大距離
	float decay;//減衰率
	int32_t isBlinnPhong;//BlinnPhongReflectionを行うかどうか
	int32_t enablePointLighting;//点光源を有効にするか
};

//スポットライト
struct SpotLight {
	Vector4 color;//ライト色
	Vector3 position;//ライトの位置
	float intensity;//輝度
	Vector3 direction;//スポットライトの方向
	float distance;//ライトの届く最大距離
	float decay;//減衰率
	float cosAngle;//スポットライトの余弦
	float cosFalloffStart;//
	int32_t isBlinnPhong;//BlinnPhongReflectionを行うかどうか
	int32_t enableSpotLighting;//点光源を有効にするか
	float padding[2];
};

//マテリアルデータ
struct MaterialData {
	std::string textureFilePath;
	uint32_t srvIndex;
};


//モデルデータの構造体
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};

//テキストのオブジェクトデータ
struct TextObjectData {
	std::vector<VertexData>vertices;
	std::string textKey;
};

//カメラのデータの構造体
struct CameraForGPU {
	Vector3 worldPosition;
	float padding;
};

//リムライトの構造体
struct RimLight {
	Vector4 color;//リムライトの色
	float power; //リムライトの強さ
	float outLinePower; //リムライトの外側の強さ
	float softness;//リムライトの柔らかさ
	int32_t enableRimLighting; //リムライトを有効にするか
};

//列挙型
enum class Transform3dMode :uint32_t {
	kNormal,
	kBilboard,
};