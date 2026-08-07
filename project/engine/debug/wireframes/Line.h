#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
namespace debugDraw {
	class Line :public BaseShape {
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Line();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Line();

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