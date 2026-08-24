#pragma once
#include "RenderData.h"
#include "RenderingData.h"
#include "DebugDrawRenderData.h"
#include "BlendMode.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <cstdint>
#include <memory>

//前方宣言
class DirectXBase;
class DirectXBase;
class TextureManager;
class Camera;
class GraphicsPipeline;

/// <summary>
/// 形
/// </summary>
namespace debugDraw {
	class BaseShape {
	private://エイリアステンプレート
		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		BaseShape();

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~BaseShape();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="directXBase">DirectXの基盤部分</param>
		/// <param name="camera">カメラ</param>
		virtual void Initialize(DirectXBase* directXBase, Camera* camera);

		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update();

		/// <summary>
		/// 描画する用のカメラを設定
		/// </summary>
		/// <param name="camera">カメラ</param>
		void SetRenderCamera(Camera* camera);

		/// <summary>
		/// カラーの設定
		/// </summary>
		/// <param name="color">色</param>
		void SetColor(const Vector4& color);

		/// <summary>
		/// カラーの取得
		/// </summary>
		/// <returns>色</returns>
		Vector4 GetColor();

		/// <summary>
		/// 描画データの取得
		/// </summary>
		/// <returns>描画データ</returns>
		const DebugDrawRenderData& GetRenderData();
	protected://メンバ関数
		/// <summary>
		/// 頂点データの設定
		/// </summary>
		virtual void SettingVertexData() = 0;

		/// <summary>
		/// インデックスの設定
		/// </summary>
		virtual void SettingIndexData() = 0;
	private://メンバ関数
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

		/// <summary>
		/// WorldTransformation行列リソースの生成
		/// </summary>
		void CreateTransformationMatrixResource();

		/// <summary>
		/// 座標の更新
		/// </summary>
		void UpdateTransform();
	private://メンバ変数
		//DirectXの基盤部分
		DirectXBase* directXBase_ = nullptr;
		//カメラ
		Camera* renderCamera_ = nullptr;
		//バッファリソース
		ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
		ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
		ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
		ComPtr<ID3D12Resource>wvpResource_ = nullptr;//ワールドビュープロジェクション
		//ワールドビュープロジェクションのデータ
		worldMatrix_* wvpData_ = nullptr;
		//バッファリソースの使い道を補足するバッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
		D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	

		//描画データ
		DebugDrawRenderData renderData_ = {};
	protected://メンバ変数
		//頂点数
		int32_t vertexCount_ = 0;
		//インデックス数
		int32_t indexCount_ = 0;
		//ワールド行列
		Matrix4x4 worldMatrix_ = {};
		//ワールド座標
		Transform transform_ = {};
		//バッファリソース内のデータを指すポインタ
		Vector4* color_ = nullptr;
		//頂点データ
		VertexData* vertexData_ = nullptr;
		//インデックスデータ
		uint32_t* indexData_ = nullptr;
		//ブレンドモード
		BlendMode blendMode_ = BlendMode::kNormal;
	};
}

