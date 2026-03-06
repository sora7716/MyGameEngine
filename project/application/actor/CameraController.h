#pragma once
#include "Vector3.h"
#include "Quaternion.h"
#include <string>

//前方宣言
class Camera;
class Player;

//移動範囲
struct Rect {
	float left = 0.0f;//左端
	float right = 1.0f;//右端
	float front = 0.0f;//手前
	float back = 1.0f;//後ろ
};

/// <summary>
/// カメラコントローラ
/// </summary>
class CameraController {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CameraController() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CameraController() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Initialize(Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 追従対象のセッター
	/// </summary>
	/// <param name="target">追従対象</param>
	void SetTarget(Player* target);

	/// <summary>
	/// 移動範囲のセッター
	/// </summary>
	/// <param name="area">移動範囲</param>
	void SetMovableArea(const Rect& area);

	/// <summary>
	/// オフセットのセッター
	/// </summary>
	/// <param name="offset">オフセット</param>
	void SetOffset(const Vector3& offset);

	/// <summary>
	/// 回転のセッター
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(const Quaternion& rotate);
private://メンバ関数
	/// <summary>
    /// 調整項目を適応
    /// </summary>
	void ApplyGlobalVariables();
private://定数
	//座標補間割合
	static inline const float kInterpolationRate = 0.1f;

	//速度の掛け率
	static inline const float kVelocityBias = 0.1f;

	//追従対象の各方向へのカメラ移動範囲
	static inline const Rect margin = { -5.0f,5.0f,5.0f,-5.0f };
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;
	//追従対象
	Player* target_ = nullptr;
	//追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = {};
	//カメラのクォータニオン
	Quaternion quaternion_ = {};
	//移動範囲
	Rect movableArea_ = { 0.0f,100.0f,0.0f,100.0f };
	//目的地
	Vector3 goalPosition_ = {};
	//FovY
	float fovY_ = 0.45f;
	//グローバル変数のグループ名
	const char* groupName_ = "cameraController";
};

