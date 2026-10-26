#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
#include <Vector3.h>
namespace wireframe {
	class Plane :public BaseShape{
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Plane();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Plane()override;

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
		/// 平面の設定
		/// </summary>
		/// <param name="plane">平面</param>
		void SetPlane(const primitiveData::Plane& plane);

		/// <summary>
		/// 平面の取得
		/// </summary>
		/// <returns>平面</returns>
		const primitiveData::Plane& GetPlane()const;
	private://メンバ変数
		/// <summary>
		/// 頂点データの設定
		/// </summary>
		void SettingVertexData()override;

		/// <summary>
		/// インデックスの設定
		/// </summary>
		void SettingIndexData()override;

		/// <summary>
		/// 垂直の処理
		/// </summary>
		/// <param name="v">ベクトル</param>
		Vector3 Perpendicular(const Vector3& v);
	private://メンバ変数
		primitiveData::Plane plane_ = {};
	};
}