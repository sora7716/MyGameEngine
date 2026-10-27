#pragma once
#include "BaseShape.h"

//前方宣言
class Camera;

/// <summary>
/// 視錐台
/// </summary>
namespace wireframe {
	class Frustum :public BaseShape{
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Frustum();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Frustum()override;

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
		std::unique_ptr<BaseShape>Clone()const override;

		/// <summary>
		/// 対象となるカメラの設定
		/// </summary>
		/// <param name="targetCamera">対象となるカメラ</param>
		void SetTargetCamera(Camera* targetCamera);
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

