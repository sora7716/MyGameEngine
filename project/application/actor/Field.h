#pragma once
#include "ActorData.h"
#include <string>
#include <vector>

//カメラ
class Object3dCommon;
class Object3d;
class Camera;

//フィールドに必要な情報の塊
struct FieldObjectDesc {
	GameObject gameObject;
	Vector3 hitBoxScale;
	ColliderState colliderState;
	Collider collider;
};

//壁に必要な情報のまとまり
struct FieldGroup {
	std::vector<FieldObjectDesc>fieldDescs;
	RenderObject renderObject;
	std::string modelName;
	int32_t objectCount;
};

/// <summary>
/// フィールド
/// </summary>
class Field {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Field();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Field();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">Object3dの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 衝突したとき
	/// </summary>
	/// <param name="colliderState">コライダーの状態</param>
	void OnCollision(ColliderState* colliderState);

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
    /// 地面に必要な情報のゲッター
    /// </summary>
    /// <returns>地面に必要な情報</returns>
	std::vector<FieldObjectDesc>& GetGroundDesc();

	/// <summary>
	/// 壁に必要な情報のゲッター
	/// </summary>
	/// <returns>壁に必要な情報</returns>
	std::vector<FieldObjectDesc>& GetWallDescs();
private://メンバ関数
	/// <summary>
	/// 壁の生成
	/// </summary>
	void CreateWall();

	/// <summary>
	/// 壁の更新
	/// </summary>
	void UpdateWall();

	/// <summary>
	/// 壁の描画
	/// </summary>
	void DrawWall();
private://メンバ変数
	//オブジェクト3dの共通部分
	Object3dCommon* object3dCommon_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;
	
	//壁に必要な情報のグループ
	FieldGroup wallGroup_ = {};

	//地面
	FieldGroup ground_ = {};
};


