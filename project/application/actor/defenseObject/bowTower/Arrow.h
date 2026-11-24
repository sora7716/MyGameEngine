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
	/// <param name="targetPos">目標地点</param>
	/// <param name="position">初期位置</param>
	/// <param name="attackPower">攻撃力</param>
	void Setup(const Vector3& targetPos,const Vector3& position,float attackPower);

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
	/// 接触した瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerEnter(BaseCollider* other)override;

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
	//目標地点
	Vector3 targetPos_ = {};
	//進む方向
	Vector3 dir_ = {};
	//目標の姿勢
	Quaternion targetRotate_ = {};
	//最初の姿勢
	Quaternion beginRotate_ = {};
	//移動速度
	float moveSpeed_ = 7.0f;
	//スピンする角度
	float spinAngle_ = 0.0f;
	//スピンする速度
	float spinSpeed_ = 20.0f;
	//生存時間
	float lifeTime_ = 0.0f;
	//最大で生存できる時間
	float maxLifetime_ = 3.0f;
	//攻撃力
	float attackPower_ = 0.0f;
};

