#pragma once
#include "Component.h"

//前方宣言
class GameObject;

//ディフェンス用のオブジェクトの回転させる
class DefenseRotationComponent :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit DefenseRotationComponent(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DefenseRotationComponent()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="targetGameObject">回転させる対象のオブジェクト</param>
	void Setup(GameObject* targetGameObject);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry);

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const;

	/// <summary>
	/// 回転方向を設定
	/// </summary>
	/// <param name="rotateDir">回転方向</param>
	void SetRotateDir(float rotateDir);

	/// <summary>
	/// 回転要求を設定
	/// </summary>
	/// <param name="request">要求</param>
	void SetRotationRequest(bool request);

	/// <summary>
	/// 実行するかの設定
	/// </summary>
	/// <param name="isRunning">実行するか</param>
	void SetIsRunning(bool isRunning);
private://定数
	//回転する基準の角度(度)
	static inline float kRotationStep = 90.0f;
private://メンバ変数
	//回転対象のGameObject
	GameObject* targetGameObject_ = nullptr;
	//回転方向
	float rotateDir_ = 0.0f;
	//角度(度)
	float degreeAngleY_ = 0.0f;
	//回転の要求
	bool rotationRequest_ = false;
	//回転を行うか
	bool isRotate_ = false;
	//現在の経過時間(秒)
	float currentSecond_ = 0.0f;
	//回転が終わる時間(秒)
	float rotateEndSecond_ = 0.4f;
	//実行するか
	bool isRunning_ = true;
	//回転の開始位置
	float beginAngle_ = {};
	//イージングの鋭さ
	float easingSharpness_ = 10.0f;
};