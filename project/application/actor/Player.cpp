#define NOMINMAX
#include "Player.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Camera.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "func/Math.h"
#include "func/Physics.h"
#include "WorldTransform.h"
#include "Bullet.h"
#include "WireframeObject3d.h"
#include "Sprite.h"

//コンストラクタ
Player::Player() {
}

//デストラクタ
Player::~Player() {
}

//初期化
void Player::Initialize(Input* input, SpriteCommon* spriteCommon, Object3dCommon* object3dCommon, Camera* camera, const std::string& modelName) {
	//入力
	input_ = input;

	//3dオブジェクトの共通部分
	object3dCommon_ = object3dCommon;

	//カメラ
	camera_ = camera;

	//トランスフォームの初期化
	gameObject_.transformData = {
		.scale = Vector3::MakeAllOne(),
		.rotate = {},
		.translate = {}
	};
	gameObject_.velocity = { 5.0f,0.0f,5.0f };
	gameObject_.acceleration = { 0.0f,Physics::kGravity,0.0f };

	//3Dオブジェクトの生成と初期化
	renderObject_.object3d = std::make_unique<Object3d>();
	renderObject_.object3d->Initialize(object3dCommon, camera_);
	renderObject_.object3d->SetModel(modelName);
	gameObject_.isAlive = true;

	//Colliderの状態
	colliderState_.scalePtr = &gameObject_.transformData.scale;
	colliderState_.rotatePtr = &gameObject_.transformData.rotate;
	colliderState_.translatePtr = &gameObject_.transformData.translate;
	colliderState_.velocityPtr = &gameObject_.velocity;
	colliderState_.isOnGroundPtr = &gameObject_.isOnGround;

	//衝突
	collider_.owner = &colliderState_;
	collider_.isTrigger = false;
	collider_.isEnabled = true;
	collider_.bodyType = BodyType::kDynamic;
	collider_.layer = Layer::kPlayer;
	collider_.maskLayer = ToBits(Layer::kWall) | ToBits(Layer::kGround) | ToBits(Layer::kPlayer);
	collider_.onCollision = [this](ColliderState* other) {this->OnCollision(other); };

	//マテリアルの初期化
	renderObject_.material.color = { 1.0f,1.0f,1.0f,1.0f };
	renderObject_.material.enableLighting = true;
	renderObject_.material.shininess = 10.0f;
	renderObject_.material.uvMatrix = Matrix4x4::Identity4x4();

	//リムライトの初期化
	rimLight_.color = Vector4::MakeWhiteColor();
	rimLight_.power = 3.4f;
	rimLight_.outLinePower = 0.3f;
	rimLight_.softness = 0.2f;
	rimLight_.enableRimLighting = false;

	//弾
	bullet_ = std::make_unique<Bullet>();
	bullet_->Initialize(object3dCommon, camera_);
	bullet_->SetAliveRange(kBulletAliveAreaSize);
	bullet_->SetSpeed(kBulletSpeed);
	bullet_->SetSize({ kBulletSize,kBulletSize,kBulletSize });
	bullet_->SetMaxBulletCount(kBulletCount);

	//スポットライトを設定
	headlight_ = object3dCommon_->GetSpotLightPtr()[0];
	headlight_.cosAngle = 0.9f;
	headlight_.cosFolloffStart = 1.2f;
	headlight_.enableSpotLighting = true;

	//ヒットボックス
	renderObject_.hitBox = std::make_unique<WireframeObject3d>();
	renderObject_.hitBox->Initialize(object3dCommon_->GetWireframeObject3dCommon(), camera_, ModelType::kCube);
	hitBoxScale_ = Vector3::MakeAllOne() - 0.2f;

	//HPバー
	//中身
	hpBar_ = std::make_unique<Sprite>();
	hpBar_->Initialize(spriteCommon, "playerHpBar.png");
	hpBar_->SetBlendMode(BlendMode::kNormal);
	hpBarTransform_.scale = { hpBarWidth_,50.0f };
	hpBarTransform_.translate = { 440.0f,630.0f };
	hpColor_ = Vector4::ColorCodeTransform("#138A0DFF");
	//アウトライン
	hpOutLine_ = std::make_unique<Sprite>();
	hpOutLine_->Initialize(spriteCommon, "playerHpOutLine.png");
	hpOutLine_->SetBlendMode(BlendMode::kNormal);
	hpOutLineTransform_.scale = { hpBarWidth_,50.0f };
	hpOutLineTransform_.translate = { 440.0f,630.0f };
}

//更新
void Player::Update() {
	//死亡判定
	Dead();

	//ダメージクールタイムを減らす
	if (damageCoolTime_ > 0.0f) {
		damageCoolTime_ -= Math::kDeltaTime;
	}

	//移動
	Move();

	//ジャンプ
	Jump();

	//速度と加速度を位置に適応
	IntegrateMotion();

	////移動の制限
	//gameObject_.transformData.translate.x = std::clamp(gameObject_.transformData.translate.x, 11.0f, 26.0f);
	//gameObject_.transformData.translate.z = std::clamp(gameObject_.transformData.translate.z, 3.0f, 29.0f);
	//下に行き過ぎないように制限
	//gameObject_.transformData.translate.y = std::max(gameObject_.transformData.translate.y, 0.0f);

	//プレイヤーが落ちたら
	if (gameObject_.transformData.translate.y < -10.0f) {
		gameObject_.isAlive = false;
	}

	//地面にいるかどうか
	if (gameObject_.transformData.translate.y <= 0.0f) {
		gameObject_.isOnGround = true;
	}

	//攻撃
	Attack();

	//マテリアルのセット
	renderObject_.object3d->GetModel()->SetMaterial(renderObject_.material);

	//リムライトのセット
	renderObject_.object3d->GetModel()->SetRimLight(rimLight_);

	//ヘッドライトの更新
	HeadlightUpdate();

	//視点
	//LookDirection();

	//トランスフォームを設定
	renderObject_.object3d->SetTransformData(0, gameObject_.transformData);

	//3Dオブジェクトの更新
	renderObject_.object3d->Update();

	//ヒットボックスの更新
	renderObject_.hitBox->SetTranslate(0, GetWorldPos());
	renderObject_.hitBox->SetRotate(0, gameObject_.transformData.rotate);
	renderObject_.hitBox->SetScale(0, hitBoxScale_);
	renderObject_.hitBox->Update();

	//HP
	hpBar_->SetTransformData(hpBarTransform_);
	hpBar_->SetColor(hpColor_);
	hpBar_->Update();
	hpOutLine_->SetTransformData(hpOutLineTransform_);
	hpOutLine_->Update();
}

//描画
void Player::Draw() {
	//3Dオブジェクトの描画
	renderObject_.object3d->Draw();
	//弾の描画
	bullet_->Draw();
	//ヒットボックスの描画
	renderObject_.hitBox->Draw();
	//HP
	//hpBar_->Draw();
	//hpOutLine_->Draw();
}

//デバッグ用
void Player::Debug() {
#ifdef USE_IMGUI
	//ImGui::DragFloat3("direction", &gameObject_.direction.x, 0.1f);
	//ImGui::DragFloat3("acceleration", &gameObject_.acceleration.x, 0.1f);
	//ImGui::DragFloat3("rotate", &gameObject_.transformData.rotate.x, 0.1f);
	//ImGui::Checkbox("isMovingToAnchor", &isMovingToAnchor_);
	ImGui::DragFloat3("trasnalate", &gameObject_.transformData.translate.x, 0.1f);
	ImGui::DragFloat3("velocity", &gameObject_.velocity.x, 0.1f);
	ImGui::ColorEdit4("color", &rimLight_.color.x);
	ImGui::DragFloat("power", &rimLight_.power, 0.1f);
	ImGui::DragFloat("outLinePower", &rimLight_.outLinePower, 0.1f);
	ImGui::DragFloat("softness", &rimLight_.softness, 0.1f);
	ImGui::Checkbox("collider.isTrigger", &collider_.isTrigger);
	ImGui::Checkbox("isOnGround", &gameObject_.isOnGround);
#endif // USE_IMGUI
}

//衝突したら
void Player::OnCollision(ColliderState* other) {
	if (other->tag == Tag::kEnemy) {
		//攻撃を受ける
		//Damage();
		if (other->translatePtr->y < GetWorldPos().y - gameObject_.transformData.scale.y) {
			gameObject_.velocity.y = 5.0f;
		} else {
			gameObject_.isAlive = false;
		}
	} else if (other->tag == Tag::kGoal) {
		isGoal_ = true;
	}
}

//カメラのセッター
void Player::SetCamera(Camera* camera) {
	renderObject_.object3d->SetCamera(camera);
	renderObject_.hitBox->SetCamera(camera);
}

//位置のセッター
void Player::SetPosition(const Vector3& positBulletion) {
	gameObject_.transformData.translate = positBulletion;
}

//速度のセッター
void Player::SetVelocity(const Vector3& velocity) {
	gameObject_.velocity = velocity;
}

//地面の上にいるかのフラグのセッター
void Player::SetIsOnGround(bool isOnGround) {
	gameObject_.isOnGround = isOnGround;
}

//地面の上にいるかのフラグのゲッター
bool Player::IsOnGround() {
	return gameObject_.isOnGround;
}

//オブジェクト3dのゲッター
Vector3 Player::GetWorldPos() const {
	return renderObject_.object3d->GetWorldPos(0);
}

//トランスフォームデータのゲッター
TransformData Player::GetTransformData()const {
	return gameObject_.transformData;
}

//速度のゲッター
Vector3 Player::GetVelocity() const {
	return gameObject_.velocity;
}

//弾のゲッター
Bullet* Player::GetBullet()const {
	return bullet_.get();
}

//生存フラグのゲッター
bool Player::IsAlive() {
	return gameObject_.isAlive;
}

//ゴールしたかのフラグのゲッター
bool Player::IsGoal() {
	return isGoal_;
}

Collider& Player::GetCollider() {
	return collider_;
}

//移動
void Player::Move() {
#ifdef _DEBUG
	//前後
	if (input_->PressKey(DIK_UP)) {
		gameObject_.direction.z = 1.0f;
	} else if (input_->PressKey(DIK_DOWN)) {
		gameObject_.direction.z = -1.0f;
	} else {
		gameObject_.direction.z = 0.0f;
	}

	//左右
	if (input_->PressKey(DIK_RIGHT)) {
		gameObject_.direction.x = 1.0f;
	} else if (input_->PressKey(DIK_LEFT)) {
		gameObject_.direction.x = -1.0f;
	} else {
		gameObject_.direction.x = 0.0f;
	}
#endif // _DEBUG

	//XboxPadの平行移動
	if (input_->IsXboxPadConnected(xBoxPadNumber_)) {
		if (std::fabs(input_->GetXboxPadLeftStick(xBoxPadNumber_).x) > 0.0f
			|| std::fabs(input_->GetXboxPadLeftStick(xBoxPadNumber_).y) > 0.0f) {
			gameObject_.direction.x = input_->GetXboxPadLeftStick(xBoxPadNumber_).x;
			gameObject_.direction.z = input_->GetXboxPadLeftStick(xBoxPadNumber_).y;
		} else {
			gameObject_.direction.x = 0.0f;
			gameObject_.direction.z = 0.0f;
		}
	}

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateXYZMatrix(camera_->GetRotate());

	//カメラの向いてる方向を正にする(XとZ軸限定)
	Vector3 moveDirXZ = Math::TransformNormal(Vector3(gameObject_.direction.x, 0.0f, gameObject_.direction.z), rotMat);

	//Y軸のそのまま
	gameObject_.direction = { moveDirXZ.x,gameObject_.direction.y,moveDirXZ.z };

	//移動させる
	gameObject_.velocity.x = gameObject_.direction.Normalize().x * kMoveSpeed;
	gameObject_.velocity.z = gameObject_.direction.Normalize().z * kMoveSpeed;
}

//攻撃
void Player::Attack() {
	//弾の発射
	bullet_->SetShootingPosition(GetWorldPos());
	bullet_->SetSourceWorldMatrix(renderObject_.object3d->GetWorldMatrix(0));
	bullet_->Fire(input_->TriggerXboxPad(xBoxPadNumber_, XboxInput::kRT));
	bullet_->Update();
}

//ジャンプ
void Player::Jump() {
	if (gameObject_.isOnGround) {
		//地面にいたらジャンプできるようにする
		if (input_->TriggerXboxPad(xBoxPadNumber_, XboxInput::kA)) {
			//Y軸に初速を代入
			gameObject_.velocity.y = kJumpSpeed;
			//地面にいるかどうかのフラグをfalse
			gameObject_.isOnGround = false;
		}

#ifdef _DEBUG
		if (input_->TriggerKey(DIK_SPACE)) {
			//Y軸に初速を代入
			gameObject_.velocity.y = kJumpSpeed;
			//地面にいるかどうかのフラグをfalse
			gameObject_.isOnGround = false;
		}
#endif // _DEBUG

	}
}

//速度と加速度を位置に適応
void Player::IntegrateMotion() {
	//加速度を適応
	gameObject_.velocity += gameObject_.acceleration * Math::kDeltaTime;
	//速度を適応
	gameObject_.transformData.translate += gameObject_.velocity * Math::kDeltaTime;
}

//ダメージを受ける
void Player::Damage() {
	//ダメージクールタイムが0だったら
	if (damageCoolTime_ <= 0.0f) {
		//Hpバーに適応
		hpBarTransform_.scale.x -= hpBarWidth_ / static_cast<float>(kMaxHpCount);
		//ダメージクールタイムを設定
		damageCoolTime_ = kMaxDamageCoolTime;
	}
}

//死亡
void Player::Dead() {
	if (hpBarTransform_.scale.x <= 0) {
		gameObject_.isAlive = false;
	}
}

//ヘッドライトの更新
void Player::HeadlightUpdate() {
	//ライトの位置をプレイヤーの前に設定
	Vector3 headlightOffset = { 0.0f, 0.0f, 0.5f };
	headlight_.position = GetWorldPos() + headlightOffset;
	// ローカル空間での“前”の向き
	Vector3 localForward = { 0.0f, 0.0f, 1.0f };

	//プレイヤーのワールド行列を取得
	Matrix4x4 world = renderObject_.object3d->GetWorldMatrix(0);

	//前方向ベクトルだけをワールド空間に変換
	Vector3 worldForward = Math::TransformNormal(localForward, world);

	//正規化してライトの方向に設定
	headlight_.direction = worldForward.Normalize();
	//headlight_.intensity = 30.0f;
	//ライトのセッター
	object3dCommon_->SetSpotLightList(0, headlight_);
}

//視点
void Player::LookDirection() {
	// XboxPad
	if (input_->IsXboxPadConnected(xBoxPadNumber_)) {

		float stickX = input_->GetXboxPadRighttStick(xBoxPadNumber_).x; // -1..1

		// デッドゾーン（スティックの微妙なブレ対策）
		const float deadZone = 0.15f;
		if (std::fabs(stickX) < deadZone) {
			stickX = 0.0f;
		} else {
			// デッドゾーン分を詰めて 0..1 に戻す（好み）
			stickX = (stickX - copysignf(deadZone, stickX)) / (1.0f - deadZone);
		}

		// 回転速度（ラジアン/秒） 例：180度/秒
		const float yawSpeed = Math::kPi; // PI rad/s = 180deg/s

		gameObject_.transformData.rotate.y += stickX * yawSpeed * Math::kDeltaTime;
	}

	// 必要なら 0..2PI に丸める（巨大化防止）
	const float twoPi = Math::kPi * 2.0f;
	if (gameObject_.transformData.rotate.y > twoPi) gameObject_.transformData.rotate.y -= twoPi;
	if (gameObject_.transformData.rotate.y < 0.0f)  gameObject_.transformData.rotate.y += twoPi;

	renderObject_.object3d->SetRotate(0, gameObject_.transformData.rotate);
}
