#pragma once
#include "RenderingData.h"
#include "LightingData.h"
#include "Vector4.h"
#include <cstdint>
#include <string>
#include <vector>
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"

//頂点データ
struct VertexData{
	Vector4 position = {};//直行座標
	Vector2 texcoord = {};//UV座標
	Vector3 normal = {};//法線
};

//マテリアル
struct Material{
	Vector4 color = Vector4::MakeWhiteColor();//色
	int32_t enableLighting = 0;//ライティングするかどうかのフラグ
	float padding1[3] = {};
	Matrix4x4 uvMatrix = Matrix4x4::Identity4x4();//UVTransform
	float shininess = 1.0f;//光沢度
	float environmentCoefficient = 0.0f;//映り込み度を調整
	float padding2[2] = {};
};

//マテリアル
struct MaterialForSprite{
	Vector4 color = Vector4::MakeWhiteColor();//色
	Matrix4x4 uvMatrix = Matrix4x4::Identity4x4();//UVTransform
};

//マテリアルデータ
struct MaterialTexturePaths{
	std::string textureFilePath = "";
	std::string environmentMap = "";
};

//メッシュデータ
struct MeshData{
	std::vector<VertexData>vertices;
	std::vector<uint32_t>indices;
	uint32_t materialIndex = 0;
};

//モデルデータの構造体
struct ModelData{
	std::vector<MeshData> meshDatas;
	std::vector<MaterialTexturePaths> materialTexturePaths;
	Node rootNode = {};
};

//テキストのオブジェクトデータ
struct TextObjectData{
	std::vector<VertexData>vertices;
	std::string textKey = "";
};

//カメラのデータの構造体
struct CameraForGPU{
	Vector3 worldPosition = {};
	float padding = 0.0f;
	Matrix4x4 viewProjection = Matrix4x4::Identity4x4();
};

//リムライトの構造体
struct RimLight{
	Vector4 color = Vector4::MakeWhiteColor();//リムライトの色
	float power = 0.0f; //リムライトの強さ
	float outLinePower = 0.0f; //リムライトの外側の強さ
	float softness = 0.0f;//リムライトの柔らかさ
	int32_t enableRimLighting = 0; //リムライトを有効にするか
};

//描画時のトランスフォームモード
enum class RenderTransformMode :uint32_t{
	kNormal,
	kBillboard,
};

//テクスチャデータ
struct TextureData{
	DirectX::TexMetadata metadata = {};//画像の幅や高さなどの情報
	Microsoft::WRL::ComPtr<ID3D12Resource>resource = {};//テクスチャリソース
	uint32_t srvIndex;//SRVインデックス
	Microsoft::WRL::ComPtr<ID3D12Resource>intermediateResource = nullptr;//アップロードするリソース
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU = {};//SRV作成時に必要なCPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU = {};//描画コマンドに必要なGPUハンドル

	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COPY_DEST; // ←生成直後はこれ
};