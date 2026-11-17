#pragma once
#include "Component.h"

//前方宣言
class FollowCameraController;

/// <summary>
/// カメラコントロール用のコンポーネント
/// </summary>
class CameraControlComponent :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit CameraControlComponent(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CameraControlComponent()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="followCamera">追従カメラ</param>
	/// <param name="followSpeed">追従速度</param>
	void Setup(FollowCameraController* followCamera,float followSpeed);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry);

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const;

	/// <summary>
	/// ピッチの設定
	/// </summary>
	/// <param name="pitch">ピッチ</param>
	void SetPitch(float pitch);

	/// <summary>
	/// ヨーの設定
	/// </summary>
	/// <param name="yaw">ヨー</param>
	void SetYaw(float yaw);
private://メンバ変数
	//仰角
	float pitch_ = 0.0f;
	//方位角
	float yaw_ = 0.0f;
	//追従速度
	float followSpeed_ = 0.0f;
	//追従カメラ
	FollowCameraController* followCamera_ = nullptr;
};

