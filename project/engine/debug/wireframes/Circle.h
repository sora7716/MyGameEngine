#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
#include <memory>
namespace wireframe {
	class Circle : public BaseShape{
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Circle();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Circle()override;

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
		/// <returns>インスタンス</returns>
		std::unique_ptr<BaseShape>Clone()const override;

		/// <summary>
		/// 円のセッター
		/// </summary>
		void SetCircle(const primitiveData::Circle& circle);

		/// <summary>
		/// 円のゲッター
		/// </summary>
		/// <returns>円</returns>
		primitiveData::Circle GetCircle();
	private://メンバ変数
		/// <summary>
		/// 頂点データの設定
		/// </summary>
		void SettingVertexData()override;

		/// <summary>
		/// インデックスの設定
		/// </summary>
		void SettingIndexData()override;
	private://メンバ変数
		//円
		primitiveData::Circle circle_ = {};
	};
}