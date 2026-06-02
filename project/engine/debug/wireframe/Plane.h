#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"
namespace Primitive {
	class Plane :public BaseShape {
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Plane();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Plane();

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
		void SetOBB(const PrimitiveData::OBB& obb);

		/// <summary>
		/// OBBのゲッター
		/// </summary>
		/// <returns>OBB</returns>
		PrimitiveData::OBB GetOBB();

		/// <summary>
		/// AABBのゲッター
		/// </summary>
		/// <returns>AABB</returns>
		PrimitiveData::AABB GetAABB();
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
		PrimitiveData::OBB obb_ = {};
	};
}