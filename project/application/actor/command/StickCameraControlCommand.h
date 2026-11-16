#pragma once
#include "IAxisCommand.h"

//前方宣言
class CameraControlComponent;

class StickCameraControlCommand :public IAxisCommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StickCameraControlCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~StickCameraControlCommand()override;

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="axis">入力値(-1~1)</param>
	void Execute(const Vector2& axis)override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <returns></returns>
	std::unique_ptr<IAxisCommand>Clone()const;

	/// <summary>
	/// カメラ操作用コンポーネントの設定
	/// </summary>
	/// <param name="cameraControlComponent">カメラ操作用のコンポーネント</param>
	void SetCameraControlComponent(CameraControlComponent* cameraControlComponent);
private://メンバ変数
	//カメラ操作用のコンポーネント
	CameraControlComponent* cameraControlComponent_ = nullptr;
	//ピッチ
	float pitch_ = 0.0f;
	//ヨー
	float yaw_ = 0.0f;
};