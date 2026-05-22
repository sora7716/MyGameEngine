#pragma once
#include "Camera.h"
#include <map>
#include <string>
#include <memory>

/// <summary>
/// カメラの管理
/// </summary>
class CameraManager {
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~CameraManager();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラの生成
	/// </summary>
	/// <param name="name">カメラの名前</param>
	void CreateCamera(const std::string& name);

	/// <summary>
	/// カメラを検索
	/// </summary>
	/// <param name="name">カメラの名前</param>
	/// <returns>カメラ</returns>
	Camera* FindCamera(const std::string& name);
public://PressKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PressKeyを受け取る</param>
	explicit CameraManager(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	CameraManager(const CameraManager&) = delete;
	//代入演算子を禁止
	CameraManager operator=(const CameraManager&) = delete;
private://メンバ変数
	//カメラ
	std::map<std::string, std::unique_ptr<Camera>>cameras_;
};

