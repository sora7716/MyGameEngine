#pragma once
#include "Component.h"
#include "Vector3.h"
#include "Vector2.h"

//前方宣言
class Input;

/// <summary>
/// オービットカメラ
/// </summary>
class OrbitCameraController :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <param name="input">入力</param>
	explicit OrbitCameraController(GameObject* gameObject, Input& input);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~OrbitCameraController()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// 対象の設定
	/// </summary>
	/// <param name="target">対象</param>
	void SetTarget(GameObject* target);
private://メンバ関数
	/// <summary>
	/// カメラの回転に関する操作
	/// </summary>
	void ViewRotationControl();
private://メンバ変数
	//入力
	Input& input_;
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//対象
	GameObject* target_ = nullptr;
	//回転軸
	float yaw_ = 0.0f;
	float pitch_ = 0.3f;
	//カメラとの距離
	float distance_ = 8.0f;

	//対象との距離
	Vector3 targetOffset_ = { 0.0f,0.0f,0.0f };

	//マウスの感度
	Vector2 sensitivity_ = { 0.01f,0.01f };
};

