#include "Bullet.h"
#include "engine/3d/Object3d.h"
#include "engine/camera/Camera.h"
#include "engine/math/func/Math.h"
#include "engine/debug/ImGuiManager.h"
#include "WireframeObject3d.h"
#include "engine/3d/Object3dCommon.h"

//コンストラクタ
Bullet::Bullet() {
}

//デストラクタ
Bullet::~Bullet() {
}

//初期化
void Bullet::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	//3Dオブジェクトの共通部分を記録
	object3dCommon_ = object3dCommon;

	//カメラを記録
	camera_ = camera;

	//ヒットボックスのサイズを設定
	hitBoxScale_ = { 0.25f,0.25f,0.25f };

	//弾の最大数を設定
	bulletDatas_.resize(kMaxBulletCount);

	for (BulletData& bullet : bulletDatas_) {
		bullet.renderObject.object3d = std::make_unique<Object3d>();
		bullet.renderObject.object3d->Initialize(object3dCommon_, camera);
		bullet.renderObject.hitBox = std::make_unique<WireframeObject3d>();
		bullet.renderObject.hitBox->Initialize(object3dCommon_->GetWireframeObject3dCommon(), camera_, ModelType::kCube);
	}
}

//弾を発射する
void Bullet::Fire(bool isShooting) {
	//弾を発射しない場合は早期リターン
	if (!isShooting) {
		return;
	}

	//弾を生成
	for (BulletData& bullet : bulletDatas_) {
		if (!bullet.gameObject.isAlive) {
			CreateBullet(bullet);
			break;
		}
	}
}

//更新
void Bullet::Update() {
	for (BulletData& bulletData : bulletDatas_) {
		if (bulletData.gameObject.isAlive) {
			//飛ばす
			bulletData.gameObject.transformData.translate += bulletData.gameObject.velocity;

			//弾の情報を設定
			bulletData.renderObject.object3d->SetTransformData(0,bulletData.gameObject.transformData);
			bulletData.renderObject.object3d->GetModel()->SetMaterial(bulletData.renderObject.material);

			//弾の生存させるか
			Vector3 currentPos = bulletData.gameObject.transformData.translate;
			float length = (currentPos - bulletData.shootingPoint).Length();
			if (bulletData.aliveRange < length) {
				bulletData.gameObject.isAlive = false;
				continue;
			}

			//弾の更新
			bulletData.renderObject.object3d->Update();

			//ヒットボックスの更新
			bulletData.renderObject.hitBox->SetTranslate(0, bulletData.gameObject.transformData.translate);
			bulletData.renderObject.hitBox->SetRotate(0,bulletData.gameObject.transformData.rotate);
			bulletData.renderObject.hitBox->SetScale(0,hitBoxScale_);
			bulletData.renderObject.hitBox->Update();
		}
	}
}

//衝突したときの
void Bullet::OnCollision(uint32_t index) {
	bulletDatas_[index].gameObject.isAlive = false;
}

//描画
void Bullet::Draw() {
	//弾の描画
	for (const BulletData& bulletData : bulletDatas_) {
		if (bulletData.gameObject.isAlive) {
			bulletData.renderObject.object3d->Draw();
			bulletData.renderObject.hitBox->Draw();
		}
	}
}

//発射地点のセッター
void Bullet::SetShootingPosition(const Vector3& shootingPosition) {
	shootingPosition_ = shootingPosition;
}

//弾を保持しているオブジェクトのワールド行列セッター
void Bullet::SetSourceWorldMatrix(const Matrix4x4& sourceWorldMatrix) {
	sourceWorldMatrix_ = sourceWorldMatrix;
}

//スピードのセッター
void Bullet::SetSpeed(float speed) {
	speed_ = speed;
}

//サイズのセッター
void Bullet::SetSize(const Vector3& size) {
	size_ = size;
}

//角度のセッター
void Bullet::SetAngle(const Vector3& angle) {
	angle_ = angle;
}

//生存範囲のセッター
void Bullet::SetAliveRange(float aliveRange) {
	aliveRange_ = aliveRange;
}

//弾の発射できる最大値のセッター
void Bullet::SetMaxBulletCount(uint32_t maxBulletCount) {
	maxBulletCount_ = maxBulletCount;
}

//モデルのセッター
void Bullet::SetModel(const std::string& modelName) {
	modelName_ = modelName;
}

//弾のデータリストのゲッター
std::vector<BulletData>& Bullet::GetBulletData() {
	return bulletDatas_;
}

//弾の生成
void Bullet::CreateBullet(BulletData&bulletData) {
	bulletData.gameObject.transformData.scale = Vector3::MakeAllOne();
	bulletData.gameObject.isAlive = true;
	//弾の位置を設定
	bulletData.gameObject.transformData.translate = shootingPosition_;
	//射撃地点を取得
	bulletData.shootingPoint = bulletData.gameObject.transformData.translate;

	//マテリアルの初期化
	bulletData.renderObject.material.color = { 1.0f,1.0f,1.0f,1.0f };
	bulletData.renderObject.material.enableLighting = true;
	bulletData.renderObject.material.shininess = 10.0f;
	bulletData.renderObject.material.uvMatrix = Matrix4x4::Identity4x4();

	//3Dモデルの生成
	bulletData.aliveRange = aliveRange_;
	bulletData.renderObject.object3d->SetModel(modelName_);

	//サイズと角度の設定
	bulletData.gameObject.transformData.scale = size_;
	bulletData.gameObject.transformData.rotate = angle_;

	//弾の速度を設定
	bulletData.gameObject.velocity = { 0.0f,0.0f,speed_ };
	bulletData.direction = Math::TransformNormal(bulletData.gameObject.velocity, sourceWorldMatrix_);
	bulletData.gameObject.velocity = bulletData.direction * Math::kDeltaTime;
}