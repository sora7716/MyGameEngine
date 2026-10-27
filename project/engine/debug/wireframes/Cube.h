#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
#include <memory>
namespace wireframe {
	class Cube :public BaseShape{
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Cube();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Cube()override;

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