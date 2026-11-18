#pragma once
#include "Component.h"
#include "Vector3.h"

//前方宣言
class RigidBody;


/// <summary>
/// ジャンプ用のコンポーネント
/// </summary>
class JumpComponent :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit JumpComponent(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~JumpComponent()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="rigidBody">リジッドボディ</param>
	/// <param name="jumpSpeed">ジャンプの速度</param>
	void Setup(RigidBody* rigidBody,float jumpSpeed);

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
	/// ジャンプする要求を設定
	/// </summary>
	/// <param name="request">リクエスト</param>
	void SetJumpRequest(bool request);
private://メンバ変数
	//リジッドボディ
	RigidBody* rigidBody_ = nullptr;
	//ジャンプスピード
	float jumpSpeed_ = 0.0f;
	//ジャンプをするかの要求
	bool jumpRequest_ = false;
};

