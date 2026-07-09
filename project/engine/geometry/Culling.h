#pragma once
#include "PrimitiveData.h"
#include <vector>
#include <memory>
//前方宣言
class Camera;
class Mesh;

/// <summary>
/// カリング
/// </summary>
class Culling{
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<Culling> Create(Camera* camera);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Culling();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Culling();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Initialize(Camera* camera);

	/// <summary>
	/// 視錐台カリングをするか
	/// </summary>
	/// <param name="aabb">AABB</param>
	/// <param name="worldMatrix">ワールド行列</param>
	/// <returns>視錐台カリングをするか</returns>
	bool IsVisibleInFrustum(const PrimitiveData::AABB& aabb, const Matrix4x4& worldMatrix);
private://メンバ変数
	//カメラ
	Camera* camera_ = nullptr;
};

