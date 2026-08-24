#pragma once
#include "RenderData.h"
#include "MatrixUtility.h"
#include "BlendMode.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>

//前方宣言
class DirectXBase;
class WorldTransform;
class SpriteCommon;

/// <summary>
/// スプライト
/// </summary>
class Sprite {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Sprite() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Sprite();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="spriteCommon">スプライトの共通部分</param>
	/// <param name="spriteName">スプライト名</param>
	void Initialize(SpriteCommon* spriteCommon, const std::string& spriteName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
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
	/// <param name="spriteName">スプライト名</param>
	void ChangeTexture(const std::string& spriteName);

	/// <summary>
	/// UVの座標変換の更新
	/// </summary>
	/// <param name="uvTransform">uv座標</param>
	void UpdateUVTransform(RectTransform uvTransform);

	/// <summary>
	/// 色のゲッター
	/// </summary>
	/// <returns>色</returns>
	const Vector4& GetColor()const;

	/// <summary>
	/// 色のセッター
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// トランスフォームのセッター
	/// </summary>
	/// <param name="transformData">トランスフォームデータ(scaleは100,100に初期設定してる)</param>
	void SetTransformData(const RectTransform& transformData);

	/// <summary>
	/// ブレンドモードのセッター
	/// </summary>
	void SetBlendMode(BlendMode blendMode);
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
	/// インデックスデータの初期化
	/// </summary>
	void InitializeIndexData();

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
	//テクスチャ番号
	std::string spriteName_ = {};
	Transform transform_ = { {100.0f,100.0f ,1.0f},{},{} };//トランスフォームの情報
	//DirectXの基盤
	DirectXBase* directXBase_ = nullptr;
	
	//スプライトの共通部分
	SpriteCommon* spriteCommon_ = nullptr;

	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
	
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;//頂点
	uint32_t* indexData_ = nullptr;//インデックス
	MaterialForSprite* materialData_ = nullptr;//マテリアル
	
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス
	
	//ワールドトランスフォーム
	WorldTransform* worldTransform_ = nullptr;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;
};
