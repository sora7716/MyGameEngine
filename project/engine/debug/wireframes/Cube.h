#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
#include <memory>
namespace debugDraw {
	class Cube :public BaseShape {
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="gameObject">ゲームオブジェクト</param>
		explicit Cube(GameObject* gameObject);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Cube();

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
	};
}