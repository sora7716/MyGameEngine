#pragma once
#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#endif // USE_IMGUI
#include "Vector3.h"
#include "PrimitiveData.h"
#include "RenderingData.h"
#include <string>
#include <memory>
//前方宣言
class WinApi;
class DirectXBase;
class SRVManager;

/// <summary>
/// ImGuiの管理
/// </summary>
class ImGuiManager{
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <param name="winApi">ウィンドウズアプリケーション</param>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVマネージャー</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<ImGuiManager>Create(ConstructorKey key, [[maybe_unused]] WinApi* winApi, [[maybe_unused]] DirectXBase* directXBase, [[maybe_unused]] SRVManager* srvManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit ImGuiManager(ConstructorKey);

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
	void Initialize([[maybe_unused]] WinApi* winApi, [[maybe_unused]] DirectXBase* directXBase, [[maybe_unused]] SRVManager* srvManager);

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
	/// OBBデータ用のImGui
	/// </summary>
	/// <param name="obb">obb</param>
	static void DragOBB([[maybe_unused]] primitiveData::OBB& obb);

	/// <summary>
	/// 円用のImGui
	/// </summary>
	/// <param name="circle">円</param>
	static void DragCircle([[maybe_unused]] primitiveData::Circle& circle);

	/// <summary>
	/// 球用のIｍGui
	/// </summary>
	/// <param name="sphere">球</param>
	static void DragSphere([[maybe_unused]] primitiveData::Sphere& sphere);

	/// <summary>
	/// int型でcheckBoxを表示する
	/// </summary>
	/// <param name="label">ラベル</param>
	/// <param name="frag">フラグ</param>
	/// <returns>チェックフラグの状態</returns>
	static bool CheckBoxToInt([[maybe_unused]] const std::string& label, [[maybe_unused]] int32_t& frag);

	/// <summary>
	/// 4x4の行列の表示
	/// </summary>
	/// <param name="matrix">行列</param>
	/// <param name="label">ラベル</param>
	static void Matrix4x4Text([[maybe_unused]] const Matrix4x4& matrix, [[maybe_unused]] const char* label);

	/// <summary>
	/// 3次元ベクトルの表示
	/// </summary>
	/// <param name="vector">ベクトル</param>
	/// <param name="label">ラベル</param>
	static void Vector3Text([[maybe_unused]] const Vector3& vector, [[maybe_unused]] const char* label);

	/// <summary>
	/// クオータニオンの表示
	/// </summary>
	/// <param name="quaternion">クオータニオン</param>
	/// <param name="label">ラベル</param>
	static void QuaternionText([[maybe_unused]] const Quaternion& quaternion, [[maybe_unused]] const char* label);

	/// <summary>
	/// 浮動小数の表示
	/// </summary>
	/// <param name="num">浮動小数</param>
	/// <param name="label">ラベル</param>
	static void FloatText([[maybe_unused]] float num, [[maybe_unused]] const char* label);

	/// <summary>
	/// AABBの表示
	/// </summary>
	/// <param name="aabb">aabb</param>
	/// <param name="label">ラベル</param>
	static void AABBText([[maybe_unused]] const primitiveData::AABB& aabb, [[maybe_unused]] const char* label);
private://メンバ関数
	//デストラクタの封印
	//コピーコンストラクタ禁止
	ImGuiManager(const ImGuiManager&) = delete;
	//代入演算子の禁止
	ImGuiManager operator=(const ImGuiManager&) = delete;
private://メンバ変数
	//WindowApi
	WinApi* winApi_ = nullptr;
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
};

