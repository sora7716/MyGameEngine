#pragma once
#include "BaseShape.h"
#include "PrimitiveData.h"

//前方宣言
class Camera;

/// <summary>
/// 視錐台
/// </summary>
namespace debugDraw {
	class Frustum :public BaseShape {
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Frustum();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Frustum();

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
		/// 対象となるカメラの設定
		/// </summary>
		/// <param name="targetCamera">対象となるカメラ</param>
		void SetTargetCamera(Camera*targetCamera);
	private://メンバ変数
		/// <summary>
		/// 頂点の設定
		/// </summary>
		void SettingVertexData()override;

		/// <summary>
		/// インデックスの設定
		/// </summary>
		void SettingIndexData()override;
	private://メンバ変数
		//対象となるカメラ
		Camera* targetCamera_ = nullptr;
	};
}

