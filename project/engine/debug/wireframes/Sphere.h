#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
namespace Primitive {
	class Sphere :public BaseShape {
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Sphere();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Sphere();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="directXBase">DirectXの基盤部分</param>
		/// <param name="camera">カメラ</param>
		void Initialize(DirectXBase* directXBase, Camera* camera)override;

		/// <summary>
		/// 更新
		/// </summary>
		void Update()override;

		/// <summary>
		/// 球のセッター
		/// </summary>
		/// <param name="sphere">球</param>
		void SetSphere(const PrimitiveData::Sphere& sphere);

		/// <summary>
		/// 球のゲッター
		/// </summary>
		/// <returns>球</returns>
		PrimitiveData::Sphere GetSphere();
	private://メンバ変数
		/// <summary>
		/// 頂点データの設定
		/// </summary>
		void SettingVertexData()override;

		/// <summary>
		/// インデックスの設定
		/// </summary>
		void SettingIndexData()override;
	private://定数
		static inline const int32_t kCircleVertexCount = 32;
	private://メンバ変数
		//球
		PrimitiveData::Sphere sphere_ = {};
	};
}