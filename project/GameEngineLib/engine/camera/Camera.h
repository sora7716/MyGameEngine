#pragma once
#include "algorithms/Rendering.h"

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
	~Camera() = default;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// クォータニオンのセッター
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	void SetQuaternion(const Quaternion& quaternion);

	/// <summary>
	/// 平行移動のセッター
	/// </summary>
	/// <param name="translate">平行移動</param>
	void SetTranslate(const Vector3& translate);

	/// <summary>
	/// 水平方向視野角のセッター
	/// </summary>
	/// <param name="fovY">水平方向視野角</param>
	void SetFovY(const float fovY);

	/// <summary>
	/// アスペクト比のセッター
	/// </summary>
	/// <param name="aspectRation">アスペクト比</param>
	void SetAspectRation(const float aspectRation);

	/// <summary>
	/// ニアクリップ距離のセッター
	/// </summary>
	/// <param name="nearClip">ニアクリップ距離</param>
	void SetNearClip(const float nearClip);

	/// <summary>
	/// ファークリップ距離のセッター
	/// </summary>
	/// <param name="farClip">ファークリップ距離</param>
	void SetFarClip(const float farClip);

	/// <summary>
	/// ワールド行列のゲッター
	/// </summary>
	/// <returns>ワールド行列</returns>
	const Matrix4x4& GetWorldMatrix()const;

	/// <summary>
	/// ビュー行列のゲッター
	/// </summary>
	/// <returns>ビュー行列</returns>
	const Matrix4x4& GetViewMatrix()const;

	/// <summary>
	/// 透視投影行列のゲッター
	/// </summary>
	/// <returns>透視投影行列</returns>
	const Matrix4x4& GetProjectionMatrix()const;

	/// <summary>
	/// ビュープロジェクション行列
	/// </summary>
	/// <returns>ビュープロジェクション行列</returns>
	const Matrix4x4& GetViewProjectionMatrix()const;

	/// <summary>
	/// クォータニオンのゲッター
	/// </summary>
	/// <returns>回転</returns>
	const Quaternion& GetQuaternion()const;

	/// <summary>
	/// 平行移動のゲッター
	/// </summary>
	/// <returns>平行移動</returns>
	const Vector3& GetTranslate()const;

	/// <summary>
	/// ニアクリップ距離のゲッター
	/// </summary>
	/// <returns></returns>
	const float GetNearClip()const;

	/// <summary>
	/// ファークリップ距離のゲッター
	/// </summary>
	/// <returns></returns>
	const float GetFarClip()const;

	/// <summary>
	/// FovYのゲッター
	/// </summary>
	/// <returns>FovY</returns>
	const float GetFovY()const;
private://メンバ変数
	//ローカル座標
	TransformData transform_ = {};
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
};

