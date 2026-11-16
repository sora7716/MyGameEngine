#pragma once
#include "Component.h"
#include "Spherical.h"
#include <chrono>

/// <summary>
///	追従カメラ
/// </summary>
class FollowCameraController :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit FollowCameraController(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~FollowCameraController()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const override;

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry);

	/// <summary>
	/// ターゲットの設定
	/// </summary>
	/// <param name="target">ターゲット</param>
	void SetTarget(GameObject* target);

	/// <summary>
	/// 動径の設定
	/// </summary>
	/// <param name="radius">動径</param>
	void SetRadius(float radius);

	/// <summary>
	/// 仰角の設定
	/// </summary>
	/// <param name="theta">仰角</param>
	void SetTheta(float theta);

	/// <summary>
	/// 方位角の設定
	/// </summary>
	/// <param name="phi">方位角</param>
	void SetPhi(float phi);

	/// <summary>
	/// 球面座標の設定
	/// </summary>
	/// <param name="spherical">球面座標</param>
	void SetSpherical(const Spherical& spherical);
private://メンバ変数
	//球面座標系
	Spherical spherical_ = {};
	//ターゲット
	GameObject* target_ = nullptr;
	//過去の時間
	std::chrono::steady_clock::time_point prevTime_ = {};
	float speed_ = 0.1f;
};

