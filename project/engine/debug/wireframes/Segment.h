#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
namespace wireframe {
	class Segment :public BaseShape{
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Segment();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Segment()override;

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
		/// 線分のセッター
		/// </summary>
		/// <param name="segment">線分</param>
		void SetSegment(const primitiveData::Segment& segment);

		/// <summary>
		/// 線分のゲッター
		/// </summary>
		/// <returns>線分</returns>
		primitiveData::Segment GetSegment();
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
		//線分
		primitiveData::Segment segment_ = {};
	};
}