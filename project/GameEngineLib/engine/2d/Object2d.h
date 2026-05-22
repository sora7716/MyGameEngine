#pragma once
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include "engine/base/BlendMode.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>

//前方宣言
class DirectXBase;
class Object2dCommon;
class WorldTransform;
class Camera;

/// <summary>
/// 2Dオブジェクト
/// </summary>
class Object2d {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object2d() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object2d();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object2dCommon">2Dオブジェクトの共通部分</param>
	/// <param name="textureName">テクスチャのファイル名</param>
	void Initialize(Object2dCommon* object2dCommon, const std::string& textureName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// テキストのセッター
	/// </summary>
	/// <param name="textName">テキストのテクスチャ名</param>
	void SetText(const std::string& textName);

	/// <summary>
	/// テクスチャの変更
	/// </summary>
	/// <param name="textureName">テクスチャの名前</param>
	void ChangeTexture(std::string textureName);

	/// <summary>
	/// サイズのゲッター
	/// </summary>
	/// <returns>サイズ</returns>
	const Vector2& GetScale()const;

	/// <summary>
	/// 回転のゲッター
	/// </summary>
	/// <returns>回転</returns>
	float GetRotate()const;

	/// <summary>
	/// 位置のゲッター
	/// </summary>
	/// <returns>位置</returns>
	const Vector2& GetTranslate()const;

	/// <summary>
	/// UVのサイズのゲッター
	/// </summary>
	/// <returns>UVサイズ</returns>
	const Vector2& GetUVScale()const;

	/// <summary>
	/// UVの回転のゲッター
	/// </summary>
	/// <returns>UVの回転</returns>
	float GetUVRotate()const;

	/// <summary>
	/// UVの位置のゲッター
	/// </summary>
	/// <returns>UVの位置</returns>
	const Vector2& GetUVTranslate()const;

	/// <summary>
	/// 色のゲッター
	/// </summary>
	/// <returns>色</returns>
	const Vector4& GetColor()const;

	/// <summary>
	/// ワールドトランスフォームのゲッター
	/// </summary>
	/// <returns>ワールドトランスフォーム</returns>
	WorldTransform* GetWorldTransform();

	/// <summary>
	/// サイズのセッター
	/// </summary>
	/// <param name="scale">サイズ(100,100で初期設定)</param>
	void SetScale(const Vector2& scale);

	/// <summary>
	/// 回転のセッター
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(float rotate);

	/// <summary>
	/// 位置のセッター
	/// </summary>
	/// <param name="translate">位置</param>
	void SetTranslate(const Vector2& translate);

	/// <summary>
	/// トランスフォームのセッター
	/// </summary>
	/// <param name="transform">トランスフォ-ム(scaleは100,100で初期設定)</param>
	void SetTransformData(const Transform2dData& transform);

	/// <summary>
	/// UVのサイズのセッター
	/// </summary>
	/// <param name="scale">サイズ</param>
	void SetUVScale(const Vector2& scale);

	/// <summary>
	/// UVの回転のセッター
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetUVRotate(float rotate);

	/// <summary>
	/// UVの位置のゲッター
	/// </summary>
	/// <param name="translate">位置</param>
	void SetUVTranslate(const Vector2& translate);

	/// <summary>
	/// 色のセッター
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// 親のセッター
	/// </summary>
	/// <param name="parent">親</param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// ブレンドモードセッター
	/// </summary>
	/// <param name="blendMode"></param>
	void SetBlendMode(const BlendMode& blendMode);
private://メンバ関数
	/// <summary>
	/// 頂点データの初期化
	/// </summary>
	void InitializeVertexData();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void InitializeMaterialData();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//2Dオブジェクトの共通部分
	Object2dCommon* object2dCommon_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	//バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;//マテリアル
	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	//モデルデータ
	ModelData modelData_ = {};
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	
	//UV座標
	Transform2dData uvTransform_ = {
		.scale = { 1.0f,1.0f },
		.rotate = 0.0f,
		.translate = {0.0f,0.0f}
	};
	//ワールド座標
	Transform2dData transform_ = {
		.scale = {100.0f,100.0f},
		.rotate = 0.0f,
		.translate = {0.0f,0.0f}
	};
	//ワールドトランスフォーム
	WorldTransform* worldTransform_ = nullptr;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;
};

