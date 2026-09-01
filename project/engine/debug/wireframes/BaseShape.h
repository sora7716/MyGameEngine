#pragma once
#include "RenderData.h"
#include "RenderingData.h"
#include "DebugDrawRenderData.h"
#include "Component.h"
#include "BlendMode.h"

/// <summary>
/// 形
/// </summary>
namespace debugDraw {
	class BaseShape :public Component{
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="gameObject">ゲームオブジェクト</param>
		explicit BaseShape(GameObject* gameObject);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~BaseShape()override;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize()final override;

		/// <summary>
		/// 更新
		/// </summary>
		void Update()final override;

		/// <summary>
		/// カラーの設定
		/// </summary>
		/// <param name="color">色</param>
		void SetColor(const Vector4& color);

		/// <summary>
		/// ブレンドモードの設定
		/// </summary>
		/// <param name="blendMode">ブレンドモード</param>
		void SetBlendMode(BlendMode blendMode);

		/// <summary>
		/// 色の取得
		/// </summary>
		/// <returns>色</returns>
		const Vector4& GetColor();

		/// <summary>
		/// ブレンドモードの取得
		/// </summary>
		/// <returns>ブレンドモード</returns>
		BlendMode GetBlendMode()const;

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

		/// <summary>
		/// 形の初期化
		/// </summary>
		virtual void InitializeShape() = 0;

		/// <summary>
		/// 形の更新
		/// </summary>
		virtual void UpdateShape() = 0;

		/// <summary>
		/// 複製する際の元となる設定
		/// </summary>
		/// <param name="baseShape">元の形</param>
		void CopyBaseSetting(BaseShape& baseShape)const;
	private://メンバ関数
		/// <summary>
		/// 座標の更新
		/// </summary>
		void UpdateTransform();

		/// <summary>
		/// 描画に必要なデータのセットアップ
		/// </summary>
		void SetupRenderData();
	private://メンバ変数
		//描画データ
		DebugDrawRenderData renderData_ = {};

		//色
		Vector4 color_ = {};
		//ワールド行列
		Matrix4x4 worldMatrix_ = {};
		//ブレンドモード
		BlendMode blendMode_ = BlendMode::kNone;
	protected://メンバ変数
		//頂点数
		uint32_t vertexCount_ = 0;
		//頂点データ
		std::vector<Vector4>vertices_ = {};

		//インデックス数
		uint32_t indexCount_ = 0;
		//インデックスデータ
		std::vector<uint32_t>indices_ = {};
	};
}

