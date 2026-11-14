#pragma once
#include "Component.h"
#include "Vector3.h"

//前方宣言
class RigidBody;

/// <summary>
/// 移動用のコンポーネント
/// </summary>
class MovementComponent :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit MovementComponent(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MovementComponent()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="rigidBody">リジッドボディ</param>
	/// <param name="moveSpeed">移動速度</param>
	/// <param name="moveResponse">移動の際のレスポンスを設定</param>
	void Setup(RigidBody* rigidBody,float moveSpeed,float moveResponse);

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
	/// 方向ベクトルの追加
	/// </summary>
	/// <param name="direction">方向ベクトル</param>
	void AddDirection(const Vector3& direction);

	/// <summary>
	/// カメラオブジェクトの設定
	/// </summary>
	/// <param name="cameraObject">カメラオブジェクト</param>
	void SetCameraObject(GameObject* cameraObject);
private://メンバ変数
	//移動の際のレスポンス
	float moveResponse_ = 1.0f;
	//移動速度
	float moveSpeed_ = 0.0f;
	//方向
	Vector3 direction_ = {};
	//リジッドボディ
	RigidBody* rigidBody_ = nullptr;
	//カメラオブジェクト
	GameObject* cameraObject_ = nullptr;
};

