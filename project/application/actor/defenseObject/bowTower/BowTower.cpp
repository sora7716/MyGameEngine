#include "BowTower.h"
#include "MathUtility.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Logger.h"
#include "Collision.h"
#include "BowTowerScanState.h"
#include "BowTowerAttackState.h"
#include "BaseScene.h"
#include "ArrowSystem.h"

//コンストラクタ
BowTower::BowTower(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
BowTower::~BowTower() {
}

//セットアップ
void BowTower::Setup(ArrowSystem* arrowSystem,float scanRadius) {
	//矢の管理システムの記録
	assert(arrowSystem);
	arrowSystem_ = arrowSystem;
	//探索する範囲を設定
	scanRadius_ = scanRadius;
}

//初期化
void BowTower::Initialize() {
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();
	//object3d_を取得
	object3d_ = gameObject_->GetComponent<Object3d>();
	if(!object3d_) {
		Logger::OutputLog("object3d_が取得できませんでした");
		assert(false);
	}

	//ステートを生成
	//探索ステート
	std::unique_ptr<IState> state = nullptr;
	state = std::make_unique<BowTowerScanState>();
	state->Setup(gameObject_);
	states_.push_back(std::move(state));
	//攻撃ステート
	state = std::make_unique<BowTowerAttackState>();
	state->Setup(gameObject_);
	states_.push_back(std::move(state));
}

//更新
void BowTower::Update() {
	//対象の配列が設定されていなかったら
	if(attackTargets_.empty()) {
		Logger::OutputLog("BowTowerに攻撃対象(配列)が設定されていません");
		return;
	}

	//探索範囲を設定
	SettingScanRange();
	//攻撃範囲を設定
	SettingAttackRange();

	if(attackTarget_) {
		//攻撃範囲にターゲットがいるか
		isInScanRange_ = collision::IsCollision(scanRange_,attackTarget_->GetWorldPos());
		//ターゲットが生存していないまたは探索範囲にいなければ
		if(!attackTarget_->IsActive() || !isInScanRange_) {
			//攻撃ステートから探索ステートへ変更
			behaviorRequest_ = Behavior::kScan;
			attackTarget_ = nullptr;
		}

	} else {
		//各攻撃対象にアクセス
		for(GameObject* attackTarget : attackTargets_) {
			//攻撃対象がNullなら
			if(!attackTarget) {
				continue;
			}

			//ターゲットが非Activeだったら
			if(!attackTarget->IsActive()) {
				continue;
			}

			//探索範囲に入っているか
			isInScanRange_ = collision::IsCollision(scanRange_,attackTarget->GetWorldPos());

			//攻撃範囲に入っているか
			if(isInScanRange_) {
				//入っていた場合
				//攻撃フェーズじゃなければ
				if(behavior_ != Behavior::kAttack) {
					//攻撃対象がNullじゃなければ
					if(!attackTarget_) {
						//衝突判定
						if(collision::IsCollision(attackRange_,attackTarget->GetWorldPos())) {
							behaviorRequest_ = Behavior::kAttack;
							//現在の攻撃対象を設定
							attackTarget_ = attackTarget;
						}
					}
				}
			} else {
				//現在の振る舞いが探索じゃなければ
				if(behavior_ != Behavior::kScan) {
					//入っていなかった場合
					behaviorRequest_ = Behavior::kScan;
					//攻撃対象をリセット
					attackTarget_ = nullptr;
				}
			}
		}
	}

	//要求がNoneじゃなければ
	if(behaviorRequest_ != Behavior::kNone) {
		//要求をコピー
		behavior_ = behaviorRequest_;
		//要求をリセット
		behaviorRequest_ = Behavior::kNone;

		//新しいステートを作成
		IState* newState = states_[static_cast<uint32_t>(behavior_) - 1].get();
		newState->Enter();
		//現在のステートがnullじゃなければ
		if(currentState_) {
			currentState_->Exit();
		}
		//現在のステートに代入
		currentState_ = newState;
	}

	//現在のステートがnullじゃなければ
	if(currentState_) {
		currentState_->Update();
	}

	//矢の発射要求があったら
	if(fireRequest_) {
		//リクエストをリセット
		fireRequest_ = false;
		//矢を発射
		arrowSystem_->Fire(attackTarget_->GetWorldPos(),headModelMatrix_ * gameObject_->GetWorldMatrix());
	}
}

//プロパティ一覧
void BowTower::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
}

//複製
std::unique_ptr<Component> BowTower::Clone(GameObject* gameObject) const {
	std::unique_ptr<BowTower>cloneInstance = std::make_unique<BowTower>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定を複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string BowTower::GetTypeName() const {
	return "BowTower";
}

//攻撃対象を設定
void BowTower::SetTargets(const std::vector<GameObject*>& targets) {
	attackTargets_ = targets;
}

//矢を発射の要求を設定
void BowTower::SetFireRequest(bool request) {
	fireRequest_ = request;
}

//スキャンする半径の取得
float BowTower::GetScanRadius() const {
	return scanRadius_;
}

//攻撃範囲の角度を取得
float BowTower::GetAttackRangeDegrees() const {
	return attackRangeDegrees_;
}

//頭のアニメーションのブレンド速度の取得
float BowTower::GetHeadTurnBlendSpeed() const {
	return headTurnBlendSpeed_;
}

//攻撃範囲の取得
const primitiveData::Sector& BowTower::GetAttackRange() const {
	return attackRange_;
}

//攻撃対象の取得
GameObject* BowTower::GetAttackTarget() const {
	return attackTarget_;
}

//攻撃範囲を設定
void BowTower::SettingAttackRange() {
	//範囲を設定
	attackRange_.origin = gameObject_->GetWorldPos();
	//どれくらいの範囲かを設定
	attackRange_.radius = scanRadius_;
	//頭のモデル行列を取得
	object3d_->TryGetNodeModelMatrix("BowTower/head",headModelMatrix_);
	//頭のワールド行列を取得
	Matrix4x4 headWorldMatrix = headModelMatrix_ * gameObject_->GetWorldMatrix();
	//頭のワールド行列から前方を取得
	attackRange_.forward = mathUtility::TransformNormal(Vector3::GetBack(),headWorldMatrix);
	//範囲の角度を設定
	attackRange_.sweepAngleDegrees = attackRangeDegrees_;
}

//探索範囲を設定
void BowTower::SettingScanRange() {
	//範囲を設定
	scanRange_.origin = gameObject_->GetWorldPos();
	//どれくらいの範囲かを設定
	scanRange_.radius = scanRadius_;
	//ワールド行列から前方を取得
	scanRange_.forward = mathUtility::TransformNormal(Vector3::GetBack(),gameObject_->GetWorldMatrix());
	//範囲の角度を設定
	scanRange_.sweepAngleDegrees = maxScanRotate_ * 2.0f;
}
