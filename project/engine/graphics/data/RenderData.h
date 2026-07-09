#pragma once
#include "RenderingData.h"
#include "Vector4.h"
#include <cstdint>
#include <string>
#include <vector>


//頂点データ
struct VertexData {
	Vector4 position = {};//直行座標
	Vector2 texcoord = {};//UV座標
	Vector3 normal = {};//法線
};

//マテリアル
struct Material {
	Vector4 color = Vector4::MakeWhiteColor();//色
	int32_t enableLighting = 0;//ライティングするかどうかのフラグ
	float padding1[3] = {};
	Matrix4x4 uvMatrix = Matrix4x4::Identity4x4();//UVTransform
	float shininess = 1.0f;//光沢度
	float environmentCoefficient;//映り込み度を調整
	float padding2[2] = {};
};

//マテリアル
struct MaterialForSprite {
	Vector4 color = Vector4::MakeWhiteColor();//色
	Matrix4x4 uvMatrix = Matrix4x4::Identity4x4();//UVTransform
};

//平行光源
struct DirectionalLight {
	Vector4 color = Vector4::MakeWhiteColor();//ライトの色
	Vector3 direction = {};//ライトの向き
	float intensity = 1.0f;//輝度
	int32_t isLambert = 0;//lambertにするかどうか
	int32_t isBlinnPhong = 0;//BlinnPhongReflectionを行うかどうか
	int32_t enableDirectionalLighting = 0;//平行光源を有効にするか
};

//点光源
struct PointLight {
	Vector4 color = Vector4::MakeWhiteColor();//ライトの色
	Vector3 position = {};//ライトの位置
	float intensity = 1.0f;//輝度
	float distance = 0.0f;//ライトの届く最大距離
	float decay = 0.0f;//減衰率
	int32_t isBlinnPhong = 0;//BlinnPhongReflectionを行うかどうか
	int32_t enablePointLighting = 0;//点光源を有効にするか
};

//スポットライト
struct SpotLight {
	Vector4 color = Vector4::MakeWhiteColor();//ライト色
	Vector3 position = {};//ライトの位置
	float intensity = 1.0f;//輝度
	Vector3 direction = {};//スポットライトの方向
	float distance = 0.0f;//ライトの届く最大距離
	float decay = 0.0f;//減衰率
	float cosAngle = 0.0f;//スポットライトの余弦
	float cosFalloffStart = 0.0f;//
	int32_t isBlinnPhong = 0;//BlinnPhongReflectionを行うかどうか
	int32_t enableSpotLighting = 0;//点光源を有効にするか
	float padding[2] = {};
};

//マテリアルデータ
struct MaterialTexturePaths {
	std::string textureFilePath = "";
	std::string environmentMap = "";
};

//メッシュデータ
struct MeshData {
	std::vector<VertexData>vertices;
	std::vector<uint32_t>indices;
	uint32_t materialIndex = 0;
};


//モデルデータの構造体
struct ModelData {
	std::vector<MeshData> mesheDatas;
	std::vector<MaterialTexturePaths> material;
	Node rootNode = {};
};

//テキストのオブジェクトデータ
struct TextObjectData {
	std::vector<VertexData>vertices;
	std::string textKey = "";
};

//カメラのデータの構造体
struct CameraForGPU {
	Vector3 worldPosition = {};
	float padding = 0.0f;
};

//リムライトの構造体
struct RimLight {
	Vector4 color = Vector4::MakeWhiteColor();//リムライトの色
	float power = 0.0f; //リムライトの強さ
	float outLinePower = 0.0f; //リムライトの外側の強さ
	float softness = 0.0f;//リムライトの柔らかさ
	int32_t enableRimLighting = 0; //リムライトを有効にするか
};

//列挙型
enum class Transform3dMode :uint32_t {
	kNormal,
	kBilboard,
};