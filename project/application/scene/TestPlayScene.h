#pragma once
#include "IScene.h"
#include "Quaternion.h"
#include "func/Rendering.h"
//前方宣言
class Object3d;

/// <summary>
/// テストプレイシーン
/// </summary>
class TestPlayScene :public IScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TestPlayScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TestPlayScene()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sceneContext">シーンで必要なもの</param>
	void Initialize(const SceneContext& sceneContext)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ変数
	Camera* camera_ = nullptr;
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
	//オブジェクト3d
	std::unique_ptr<Object3d>object3d_ = nullptr;
	TransformData transformData_ = {};

	Vector3 axis = Vector3::MakeAllOne().Normalize();
	float angle = 0.44f;
	Matrix4x4 rotateMatrix = Rendering::MakeRotateAxisAngle(axis, angle);

	Quaternion q1 = { 2.0f,3.0f,4.0f,1.0f };
	Quaternion q2 = { 1.0f,3.0f,5.0f,2.0f };
	Quaternion identity = Quaternion::IdentityQuaternion();
	Quaternion conj = q1.Conjugate();
	Quaternion inv = q1.Inverse();
	Quaternion normal = q1.Normalize();
	Quaternion mul1 = q1 * q2;
	Quaternion mul2 = q2 * q1;
	float norm = q1.Norm();

	Quaternion rotation = Rendering::MakeRotateAxisAngleQuaternion(Vector3({ 1.0f,0.4f,-0.2f }).Normalize(), 0.45f);
	Vector3 pointY = { 2.1f,-0.9f,1.3f };
	Matrix4x4 rotateMat = Rendering::MakeRotateMatrix(rotation);
	Vector3 rotateByQuaternion = Rendering::RotateVector(pointY, rotation);
	Vector3 rotateByMatrix = Rendering::Transform(pointY, rotateMat);
};
