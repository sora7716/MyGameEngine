#pragma once
#include "Vector3.h"
#include <Windows.h>

// 前方宣言
class Input;
class Camera;

/// <summary>
/// ゲームカメラ
/// </summary>
class GameCamera {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameCamera() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameCamera() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="input">入力</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Input* input, Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// カメラのゲッター
	/// </summary>
	/// <returns>カメラ</returns>
	Camera* GetCamera();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// ターゲットの位置のセッター
	/// </summary>
	/// <param name="targetPos">ターゲットの位置</param>
	void SetTragetPos(const Vector3& targetPos);
private://定数
	//カメラの回転速度
	static inline const float kRotateSpeed = 5.0f;
private://メンバ変数
	//入力
	Input* input_ = nullptr;
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
	//カメラ
	Camera* camera_ = nullptr;
	//ターゲット
	Vector3 targetPos_{ 0.0f,0.0f,0.0f };
	//オフセット
	Vector3 offset_{ 0.0f,0.0f,0.0f };
	//回転
	Vector3 rotate_{ 0.0f,0.0f,0.0f };
};

