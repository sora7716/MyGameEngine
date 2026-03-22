#pragma once
#include "BaseGround.h"
/// <summary>
/// ジャンプパッド
/// </summary>
class JumpPad :public BaseGround {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	JumpPad();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~JumpPad()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">3dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera)override;
};

