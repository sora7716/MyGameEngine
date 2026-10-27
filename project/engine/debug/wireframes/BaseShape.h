#pragma once
#include "RenderData.h"
#include "RenderingData.h"
#include "WireframeRenderData.h"
#include "BlendMode.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>
#include <vector>

/// <summary>
/// 形
/// </summary>
namespace wireframe {
	class BaseShape{
	public://静的メンバ関数
		template <class T>
		static std::unique_ptr<T>Create(){
			std::unique_ptr<T>instance = std::make_unique<T>();
			instance->Initialize();
			return instance;
		}
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
		void Initialize();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// 複製
		/// </summary>
		/// <returns>コンポーネント</returns>
		virtual std::unique_ptr<BaseShape>Clone()const = 0;

		/// <summary>
		/// カラーの設定
		/// </summary>
		/// <param name="color">色</param>
		void SetColor(const Color& color);

		/// <summary>
		/// ブレンドモードの設定
		/// </summary>
		/// <param name="blendMode">ブレンドモード</param>
		void SetBlendMode(BlendMode blendMode);

		/// <summary>
		/// ローカルスケールを設定
		/// </summary>
		/// <param name="scale">スケール</param>
		void SetLocalScale(const Vector3& scale);

		/// <summary>
		/// ローカルの回転を設定
		/// </summary>
		/// <param name="rotate">回転</param>
		void SetLocalRotate(const Quaternion& rotate);

		/// <summary>
		/// ローカルの平行移動を設定
		/// </summary>
		/// <param name="translate">平行移動</param>
		void SetLocalTranslate(const Vector3& translate);

		/// <summary>
		/// 有効状態を設定
		/// </summary>
		/// <param name="isEnabled">有効状態</param>
		void SetEnabled(bool isEnabled);

		/// <summary>
		/// 色の取得
		/// </summary>
		/// <returns>色</returns>
		const Color& GetColor();

		/// <summary>
		/// ブレンドモードの取得
		/// </summary>
		/// <returns>ブレンドモード</returns>
		BlendMode GetBlendMode()const;

		/// <summary>
		/// 描画データの取得
		/// </summary>
		/// <returns>描画データ</returns>
		const WireframeRenderData& GetRenderData();

		/// <summary>
		/// 有効状態を取得
		/// </summary>
		/// <returns>有効状態</returns>
		bool IsEnabled()const;
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
		//有効か
		bool isEnabled_ = true;
		//描画データ
		WireframeRenderData renderData_ = {};
		//色
		Color color_ = {};
		//ローカルのトランスフォーム
		Transform localTransform_ = {};
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

