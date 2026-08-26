#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
#include <Vector3.h>
namespace debugDraw {
	class Plane :public BaseShape {
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="gameObject">ゲームオブジェクト</param>
		explicit Plane(GameObject* gameObject);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Plane();

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