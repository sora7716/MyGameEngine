#pragma once
#include "BaseGround.h"

/// <summary>
/// 地面
/// </summary>
class Ground :public BaseGround {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Ground();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Ground();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">3dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();
};

