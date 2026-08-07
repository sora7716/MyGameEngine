#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
namespace debugDraw {
	class Cube :public BaseShape {
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Cube();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Cube();

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
		/// OBBのセッター
		/// </summary>
		/// <param name="obb">OBB</param>
		void SetOBB(const primitiveData::OBB& obb);

		/// <summary>
		/// OBBのゲッター
		/// </summary>
		/// <returns>OBB</returns>
		primitiveData::OBB GetOBB();

		/// <summary>
		/// AABBのゲッター
		/// </summary>
		/// <returns>AABB</returns>
		primitiveData::AABB GetAABB();
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
		//OBB
		primitiveData::OBB obb_ = {};
	};
}