#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
namespace debugDraw {
	class Circle : public BaseShape{
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="gameObject">ゲームオブジェクト</param>
		explicit Circle(GameObject* gameObject);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Circle();

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