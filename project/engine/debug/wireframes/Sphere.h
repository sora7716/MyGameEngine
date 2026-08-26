#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
namespace debugDraw {
	class Sphere :public BaseShape {
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="gameObject">ゲームオブジェクト</param>
		explicit Sphere(GameObject* gameObject);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Sphere();

		/// <summary>
		/// 初期化
		/// </summary>
		void InitializeShape()override;

		/// <summary>
		/// 更新
		/// </summary>
		void UpdateShape()override;

		/// <summary>
		/// 複製
		/// </summary>
		/// <param name="gameObject">ゲームオブジェクト</param>
		/// <returns>コンポーネント</returns>
		std::unique_ptr<Component>Clone(GameObject* gameObject)const override;

		/// <summary>
		/// 球のセッター
		/// </summary>
		/// <param name="sphere">球</param>
		void SetSphere(const primitiveData::Sphere& sphere);

		/// <summary>
		/// 球のゲッター
		/// </summary>
		/// <returns>球</returns>
		primitiveData::Sphere GetSphere();
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
		primitiveData::Sphere sphere_ = {};
	};
}