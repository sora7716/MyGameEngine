#include "Arrow.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "BaseCollider.h"
#include "Health.h"
#include "Logger.h"
#include <cassert>
#include <cmath>
#include <algorithm>

//コンストラクタ
Arrow::Arrow(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
Arrow::~Arrow() {
}

//セットアップ
void Arrow::Setup(const Vector3& targetPos,const Vector3& position,float attackPower) {
	//生存タイマーの初期化
	lifeTime_ = 0.0f;
	
	//攻撃力を記録
	attackPower_ = attackPower;

	//目標地点を記録
	targetPos_ = targetPos;

	//Transformを取得
	Transform& transform = gameObject_->GetTransform();

	//サイズを設定
	transform.scale = Vector3::GetOne() / 1.5f;

	//位置を設定
	transform.translate = position;
	initialPos_ = position;//最初の位置を取得

	//向く方向を計算
	initialDistance_ = (targetPos_ - transform.translate).Length();//最初の距離を設定
	dir_ = (targetPos_ - transform.translate).Normalize();
	//ヨー
	float yaw = std::atan2(dir_.x,dir_.z);
	//XとZの水平方向を取得
	Vector3 dirXZ = {dir_.x,0.0f,dir_.z};
	//ピッチ
	float pitch = std::atan2(-dir_.y,dirXZ.Length());

	//目標の姿勢を設定
	targetRotate_ = Quaternion::EulerAngleToQuaternion({pitch,yaw,0.0f});

	//回転を適応
	transform.SetEulerAngle({0.0f,yaw,0.0f});
	//最初の姿勢を取得
	beginRotate_ = transform.rotate;

	//スピンする角度を設定
	spinAngle_ = transform.GetEulerAngle().z;
}

//初期化
void Arrow::Initialize() {
	//ゲームオブジェクトを記録
	gameObject_ = GetOwner();
}

//更新
void Arrow::Update() {
	//生存時間を計測
	lifeTime_ += mathUtility::GetDeltaTime();
	//最大で生存できる時間が過ぎたら
	if(maxLifetime_ <= lifeTime_) {
		gameObject_->SetIsActive(false);
	}

	//Transformを取得
	Transform& transform = gameObject_->GetTransform();

	//向ている方向に移動させる
	transform.translate += (moveSpeed_ * dir_) * mathUtility::GetDeltaTime();

	//動いた距離
	float traveledDistance = (gameObject_->GetWorldPos() - initialPos_).Length();

	//回転の補完する際に使用する係数
	float t = std::clamp(traveledDistance / initialDistance_,0.0f,1.0f);
	//補正をした回転
	Quaternion correctedRotation = Quaternion::Slerp(beginRotate_,targetRotate_,t);

	//常にZ軸にスピンする回転
	spinAngle_ += spinSpeed_ * mathUtility::GetDeltaTime();
	Quaternion spinRotation = Quaternion::EulerAngleToQuaternion({0.0f,0.0f,spinAngle_});

	//Transformに設定
	transform.rotate = correctedRotation * spinRotation;
}

//プロパティ一覧を登録
void Arrow::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("移動速度",moveSpeed_);
	registry->Add("スピンする速度",spinSpeed_);
	registry->Add("矢の攻撃力",attackPower_);
}

//複製
std::unique_ptr<Component> Arrow::Clone(GameObject* gameObject) const {
	std::unique_ptr<Arrow>cloneInstance = std::make_unique<Arrow>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//接触した瞬間
void Arrow::OnTriggerEnter(BaseCollider* other) {
	//敵に衝突したら
	if(other->GetOwner()->GetTag() == "Enemy") {
		gameObject_->SetIsActive(false);
		//敵にダメージ
		GameObject* enemyObject = other->GetOwner();
		//HP管理を取得
		Health* health = enemyObject->GetComponent<Health>();
		if(!health) {
			Logger::OutputLog("敵にHealthがコンポーネントされていません");
			assert(false);
		}
		//ダメージを与える
		health->ApplyDamage(attackPower_);
	}
}

//コンポーネントのタイプ名を返す
std::string Arrow::GetTypeName() const {
	return "Arrow";
}

