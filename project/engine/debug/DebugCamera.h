#pragma once
#include "Vector3.h"
#include "Vector2.h"

//前方宣言
class Camera;
class CameraManager;

/// <summary>
/// デバックカメラ
/// </summary>
class DebugCamera{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DebugCamera() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugCamera() = default;


	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="debugCamera">デバッグカメラ</param>
	void Initialize(Camera* debugCamera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラのゲッター
	/// </summary>
	/// <returns>カメラ</returns>
	Camera* GetCamera();

	/// <summary>
	/// デバックに使用する
	/// </summary>
	void Debug();
private://メンバ関数
	/// <summary>
	/// 左右移動の操作
	/// </summary>
	void StrafeControl();

	/// <summary>
	/// 上下移動の操作
	/// </summary>
	void ElevateControl();

	/// <summary>
	/// 前後移動の操作
	/// </summary>
	void DollyControl();

	/// <summary>
	/// ズーム操作
	/// </summary>
	void ZoomControl();

	/// <summary>
	/// 回転の操作
	/// </summary>
	void RotateControl();

	/// <summary>
	/// 平行移動の更新
	/// </summary>
	void TranslateUpdate();
public://定数
	//カメラ回転時のマウスの移動量に対する回転角の倍率
	static inline const float kLookRadPerCount = 1.0f / 300.0f;
	//ズーム時の移動速度の倍率
	static inline const float kZoomSpeedMagnification = 1.0f / 10.0f;
	//FovYの最小値
	static inline const float kMinFovY = 0.1f;
	//FovYの最大値
	static inline const float kMaxFovY = 2.0f;
	//カメラの移動速度
	static inline const float kMoveSpeed = 0.5f;
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;
	//X,Y,Z軸回りのローカル回転角
	Vector3 rotate_ = { 0.0f,0.0f,0.0f };
	//ローカル座標
	Vector3 translate_ = { 0.0f,0.0f,-10.0f };
	//マウスのフリック量
	Vector2 mouseFlick_ = {};
	//カメラの移動方向のベクトル
	Vector3 moveDir_ = {};
	//FovY
	float fovY_ = 0.0f;
	//デバッグモード
	bool isDebug_ = false;
};

