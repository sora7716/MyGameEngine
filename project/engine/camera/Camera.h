#pragma once
#include "MatrixUtility.h"
#include "PrimitiveData.h"
#include <memory>

class DirectXBase;

/// <summary>
/// カメラ
/// </summary>
class Camera {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Camera();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Camera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// オイラー角の設定
	/// </summary>
	/// <param name="eulerAngle">オイラー角</param>
	void SetEulerAngle(const Vector3& eulerAngle);

	/// <summary>
	/// クォータニオンの設定
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	void SetQuaternion(const Quaternion& quaternion);

	/// <summary>
	/// 平行移動の設定
	/// </summary>
	/// <param name="translate">平行移動</param>
	void SetTranslate(const Vector3& translate);

	/// <summary>
	/// 水平方向視野角の設定
	/// </summary>
	/// <param name="fovY">水平方向視野角</param>
	void SetFovY(const float fovY);

	/// <summary>
	/// アスペクト比の設定
	/// </summary>
	/// <param name="aspectRation">アスペクト比</param>
	void SetAspectRation(const float aspectRation);

	/// <summary>
	/// ニアクリップ距離の設定
	/// </summary>
	/// <param name="nearClip">ニアクリップ距離</param>
	void SetNearClip(const float nearClip);

	/// <summary>
	/// ファークリップ距離の設定
	/// </summary>
	/// <param name="farClip">ファークリップ距離</param>
	void SetFarClip(const float farClip);

	/// <summary>
	/// ワールド行列の取得
	/// </summary>
	/// <returns>ワールド行列</returns>
	const Matrix4x4& GetWorldMatrix()const;

	/// <summary>
	/// オイラー角の取得
	/// </summary>
	/// <returns>オイラー角</returns>
	const Vector3& GetEulerAngle()const;

	/// <summary>
	/// ビュー行列の取得
	/// </summary>
	/// <returns>ビュー行列</returns>
	const Matrix4x4& GetViewMatrix()const;

	/// <summary>
	/// 透視投影行列の取得
	/// </summary>
	/// <returns>透視投影行列</returns>
	const Matrix4x4& GetProjectionMatrix()const;

	/// <summary>
	/// ビュープロジェクション行列
	/// </summary>
	/// <returns>ビュープロジェクション行列</returns>
	const Matrix4x4& GetViewProjectionMatrix()const;

	/// <summary>
	/// クォータニオンの取得
	/// </summary>
	/// <returns>回転</returns>
	const Quaternion& GetQuaternion()const;

	/// <summary>
	/// 平行移動の取得
	/// </summary>
	/// <returns>平行移動</returns>
	const Vector3& GetTranslate()const;

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPos()const;

	/// <summary>
	/// 視錐台の取得
	/// </summary>
	/// <returns>視錐台</returns>
	primitiveData::Frustum& GetFrustum();

	/// <summary>
	/// ニアクリップ距離の取得
	/// </summary>
	/// <returns></returns>
	const float GetNearClip()const;

	/// <summary>
	/// ファークリップ距離の取得
	/// </summary>
	/// <returns></returns>
	const float GetFarClip()const;

	/// <summary>
	/// FovYの取得
	/// </summary>
	/// <returns>FovY</returns>
	const float GetFovY()const;

	/// <summary>
	/// アスペクト比の取得
	/// </summary>
	/// <returns></returns>
	const float GetAspectRation()const;
private://メンバ変数
	//ローカル座標
	Transform transform_ = {};
	//ワールド行列
	Matrix4x4 worldMatrix_ = {};
	//ビュー行列
	Matrix4x4 viewMatrix_ = {};
	//透視投影行列
	Matrix4x4 projectionMatrix_ = {};
	//水平方向視野角
	float fovY_ = 0.45f;
	//アスペクト比
	float aspectRation_ = 0.0f;
	//ニアクリップ距離
	float nearClip_ = 0.1f;
	//ファークリップ距離
	float farClip_ = 100.0f;
	//ビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_ = {};
	//視錐台
	primitiveData::Frustum frustum_ = {};
};

