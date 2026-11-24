#pragma once
#include "Component.h"
#include "Vector3.h"
#include "Quaternion.h"

//前方宣言
class GameObject;

/// <summary>
/// 矢
/// </summary>
class Arrow :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Arrow(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Arrow()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="target">ターゲット</param>
	/// <param name="position">初期位置</param>
	void Setup(GameObject* target,const Vector3& position);

	//初期化
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
	/// 接触した瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerEnter(BaseCollider* other)override;

	/// <summary>
	/// 離れた瞬間
	/// </summary>
	/// <param name="other"></param>
	void OnTriggerExit(BaseCollider* other)override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const;
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//最初の位置
	Vector3 initialPos_ = {};
	//最初の距離
	float initialDistance_ = {};
	//ターゲット
	GameObject* target_ = nullptr;
	//進む方向
	Vector3 dir_ = {};
	//目標の姿勢
	Quaternion targetRotate_ = {};
	//最初の姿勢
	Quaternion beginRotate_ = {};
	//移動速度
	float moveSpeed_ = 5.0f;
};

