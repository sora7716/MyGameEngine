#pragma once
#include "GameObjectData.h"
#include "ActorData.h"
#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#endif // USE_IMGUI
#include <string>
#include "Vector3.h"
#include "PrimitiveData.h"
//前方宣言
class DirectXBase;
class SRVManager;
class WinApi;

/// <summary>
/// ImGuiの管理
/// </summary>
class ImGuiManager {
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ImGuiManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApi">ウィンドウズアプリケーション</param>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVマネージャー</param>
	void Initialize(WinApi* winApi, DirectXBase* directXBase, SRVManager* srvManager);

	/// <summary>
	/// ImGuiの受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGuiの受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグで動かせるものをツリー上に配置
	/// </summary>
	/// <param name="label">ラベル</param>
	/// <param name="entityGroup">エンティティグループ</param>
	static void TreeNodeForEntityGroup(const std::string& label,EntityGroup& entityGroup);

	/// <summary>
	/// トランスフォームデータ用のImGui
	/// </summary>
	/// <param name="transfromData">トランスフォームデータ</param>
	static void DragTransform(TransformData& transfromData);

	/// <summary>
	/// OBBデータ用のImGui
	/// </summary>
	/// <param name="obb">obb</param>
	static void DragOBB(PrimitiveData::OBB& obb);

	/// <summary>
	/// 円用のImGui
	/// </summary>
	/// <param name="circle">円</param>
	static void DragCircle(PrimitiveData::Circle& circle);

	/// <summary>
	/// 球用のIｍGui
	/// </summary>
	/// <param name="sphere">球</param>
	static void DragSphere(PrimitiveData::Sphere& sphere);

	/// <summary>
	/// ゲームオブジェクトのデバッグ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	static void DebugGameObject(GameObject& gameObject);

	/// <summary>
	/// int型でcheckBoxを表示する
	/// </summary>
	/// <param name="label">ラベル</param>
	/// <param name="frag">フラグ</param>
	/// <returns>チェックフラグの状態</returns>
	static bool CheckBoxToInt(const std::string& label, int32_t& frag);

	/// <summary>
	/// 4x4の行列の表示
	/// </summary>
	/// <param name="matrix">行列</param>
	/// <param name="label">ラベル</param>
	static void Matrix4x4Text(const Matrix4x4& matrix, const char* label);

	/// <summary>
	/// 3次元ベクトルの表示
	/// </summary>
	/// <param name="vector">ベクトル</param>
	/// <param name="label">ラベル</param>
	static void Vector3Text(const Vector3& vector, const char* label);

	/// <summary>
	/// クオータニオンの表示
	/// </summary>
	/// <param name="quaternion">クオータニオン</param>
	/// <param name="label">ラベル</param>
	static void QuaternionText(const Quaternion& quaternion, const char* label);

	/// <summary>
	/// 浮動小数の表示
	/// </summary>
	/// <param name="num">浮動小数</param>
	/// <param name="label">ラベル</param>
	static void FloatText(float num, const char* label);
public://PassKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit ImGuiManager(ConstructorKey);
private://メンバ関数
	//デストラクタの封印
	//コピーコンストラクタ禁止
	ImGuiManager(const ImGuiManager&) = delete;
	//代入演算子の禁止
	ImGuiManager operator=(const ImGuiManager&) = delete;
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
};

