#include "ImGuiManager.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "MatrixUtility.h"
#include "MathUtility.h"
#include "WinApi.h"

//生成
std::unique_ptr<ImGuiManager> ImGuiManager::Create(ConstructorKey key, WinApi* winApi, DirectXBase* directXBase, SRVManager* srvManager){
	//生成
	std::unique_ptr<ImGuiManager>instance = std::make_unique<ImGuiManager>(key);
	//初期化
	instance->Initialize(winApi, directXBase, srvManager);

	return instance;
}

//コンストラクタ
ImGuiManager::ImGuiManager(ConstructorKey){}

//デストラクタ
ImGuiManager::~ImGuiManager(){
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // USE_IMGUI
}

//初期化
void ImGuiManager::Initialize([[maybe_unused]] WinApi* winApi, [[maybe_unused]] DirectXBase* directXBase, [[maybe_unused]] SRVManager* srvManager){
#ifdef USE_IMGUI
	//WindowApiを記録する
	assert(winApi);
	winApi_ = winApi;
	//DirectXの基盤部分を記録する
	assert(directXBase);
	directXBase_ = directXBase;
	//SRVの管理を記録する
	assert(srvManager);
	srvManager_ = srvManager;
	IMGUI_CHECKVERSION();
	//ImGuiのコンテキストを生成
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//ドッキング機能の有効化
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//マルチビューポートの有効化
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//フォントの設定
	ImFont* JapaneseFont = io.Fonts->AddFontFromFileTTF("engine/resources/fonts/SoraGameUI-Bold.ttf", 18.0f, nullptr,
		io.Fonts->GetGlyphRangesJapanese());
	assert(JapaneseFont);
	io.Fonts->Build();

	//スタイルのセットアップ
	SetupStyle();

	//win32の初期化
	ImGui_ImplWin32_Init(winApi->GetHwnd());
	//srvの確保
	srvManager_->Allocate();

	//DX12の初期化
	ImGui_ImplDX12_Init(
		directXBase_->GetDevice(),
		static_cast<int>(directXBase_->GetSwapChainResourceSize()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvManager_->GetDescriptorHeap(),
		srvManager_->GetCPUDescriptorHandle(0),
		srvManager_->GetGPUDescriptorHandle(0));
#endif // USE_IMGUI
}

//ImGuiの受付開始
void ImGuiManager::Begin(){
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // USE_IMGUI
}

//ImGuiの受付終了
void ImGuiManager::End(){
#ifdef USE_IMGUI
	//描画前準備
	ImGui::Render();
#endif // USE_IMGUI
}

//描画
void ImGuiManager::Draw(){
#ifdef USE_IMGUI
	ID3D12GraphicsCommandList* commandList = directXBase_->GetCommandList();
	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvManager_->GetDescriptorHeap() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), directXBase_->GetCommandList());
#endif // USE_IMGUI
}

//プラットフォームウィンドウの更新
void ImGuiManager::UpdatePlatformWindow(){
#ifdef USE_IMGUI
	//外へ出したImGuiウィンドウの作成・移動・リサイズ・破棄を行う
	ImGui::UpdatePlatformWindows();
	//追加ウィンドウへImGuiを描画して、それぞれPresentする
	ImGui::RenderPlatformWindowsDefault();
#endif // USE_IMGUI
}

//OBBデータ用のImGui
void ImGuiManager::DragOBB([[maybe_unused]] primitiveData::OBB& obb){
#ifdef USE_IMGUI
	ImGui::DragFloat3("size", &obb.size.x, 0.1f);
	static Vector3 obbRadian = {};
	ImGui::DragFloat3("rotate", &obbRadian.x, 0.1f);
	obb.quaternion = Quaternion::EulerAngleToQuaternion(obbRadian);
	ImGui::DragFloat3("center", &obb.center.x, 0.1f);
#endif // USE_IMGUI
}

//円用のImGui
void ImGuiManager::DragCircle([[maybe_unused]] primitiveData::Circle& circle){
#ifdef USE_IMGUI
	ImGui::DragFloat3("center", &circle.center.x, 0.1f);
	ImGui::DragFloat3("eulerAngle", &circle.eulerAngle.x, 0.1f);
	ImGui::DragFloat("radius", &circle.radius, 0.01f);
#endif // USE_IMGUI
}

//球用のImGui
void ImGuiManager::DragSphere([[maybe_unused]] primitiveData::Sphere& sphere){
#ifdef USE_IMGUI
	ImGui::DragFloat3("center", &sphere.center.x, 0.1f);
	ImGui::DragFloat("radius", &sphere.radius, 0.01f);
#endif // USE_IMGUI
}

//int型でcheckBoxを表示する
bool ImGuiManager::CheckBoxToInt([[maybe_unused]] const std::string& label, [[maybe_unused]] int32_t& frag){
	bool checkBox = static_cast<bool>(frag);
#ifdef USE_IMGUI
	ImGui::Checkbox(label.c_str(), &checkBox);
#endif // USE_IMGUI
	frag = static_cast<int32_t>(checkBox);
	return checkBox;
}

//4x4の行列の表示
void ImGuiManager::Matrix4x4Text([[maybe_unused]] const Matrix4x4& matrix, [[maybe_unused]] const char* label){
#ifdef USE_IMGUI
	ImGui::SeparatorText(label);
	for (int32_t i = 0; i < 4; i++){
		for (int32_t j = 0; j < 4; j++){

			ImGui::Text("%5.3f", matrix.m[i][j]);

			if (j < 3){
				ImGui::SameLine();
			}
		}
	}
#endif // USE_IMGUI
}

//3次元ベクトルの表示
void ImGuiManager::Vector3Text([[maybe_unused]] const Vector3& vector, [[maybe_unused]] const char* label){
#ifdef USE_IMGUI
	ImGui::SeparatorText(label);
	ImGui::Text("%5.3f %5.3f %5.3f", vector.x, vector.y, vector.z);
#endif // USE_IMGUI
}

//クオータニオンの表示
void ImGuiManager::QuaternionText([[maybe_unused]] const Quaternion& quaternion, [[maybe_unused]] const char* label){
#ifdef USE_IMGUI
	ImGui::SeparatorText(label);
	ImGui::Text("%5.3f %5.3f %5.3f %5.3f", quaternion.x, quaternion.y, quaternion.z, quaternion.w);
#endif // USE_IMGUI
}

//浮動小数の表示
void ImGuiManager::FloatText([[maybe_unused]] float num, [[maybe_unused]] const char* label){
#ifdef USE_IMGUI
	ImGui::SeparatorText(label);
	ImGui::Text("%5.3f", num);
#endif // USE_IMGUI
}

//AABBの表示
void ImGuiManager::AABBText([[maybe_unused]] const primitiveData::AABB& aabb, [[maybe_unused]] const char* label){
#ifdef _DEBUG
	ImGuiManager::Vector3Text(aabb.min, (static_cast<std::string>(label) + ".min").c_str());
	ImGuiManager::Vector3Text(aabb.max, (static_cast<std::string>(label) + ".max").c_str());
#endif // _DEBUG

}

//ウィンドウのスタイルのセットアップ
void ImGuiManager::SetupStyle(){
#ifdef USE_IMGUI
	ImGuiStyle& style = ImGui::GetStyle();

	// サイズ・余白
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.FramePadding = ImVec2(7.0f, 5.0f);
	style.CellPadding = ImVec2(6.0f, 4.0f);

	style.ItemSpacing = ImVec2(8.0f, 6.0f);
	style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);

	style.IndentSpacing = 20.0f;
	style.ScrollbarSize = 13.0f;
	style.GrabMinSize = 10.0f;

	// 角丸
	style.WindowRounding = 2.0f;
	style.ChildRounding = 2.0f;
	style.FrameRounding = 3.0f;
	style.PopupRounding = 3.0f;
	style.ScrollbarRounding = 3.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 3.0f;

	// Border
	style.WindowBorderSize = 1.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.FrameBorderSize = 0.0f;
	style.TabBorderSize = 0.0f;

	ImVec4* colors = style.Colors;

	// Text
	colors[ImGuiCol_Text] =
		ImVec4(0.84f, 0.85f, 0.87f, 1.00f);

	colors[ImGuiCol_TextDisabled] =
		ImVec4(0.45f, 0.47f, 0.50f, 1.00f);

	// Window
	colors[ImGuiCol_WindowBg] =
		ImVec4(0.071f, 0.082f, 0.098f, 1.00f);

	colors[ImGuiCol_ChildBg] =
		ImVec4(0.071f, 0.082f, 0.098f, 1.00f);

	colors[ImGuiCol_PopupBg] =
		ImVec4(0.090f, 0.102f, 0.122f, 1.00f);

	// Border
	colors[ImGuiCol_Border] =
		ImVec4(0.161f, 0.180f, 0.208f, 1.00f);

	colors[ImGuiCol_BorderShadow] =
		ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	// Frame / Input
	colors[ImGuiCol_FrameBg] =
		ImVec4(0.055f, 0.067f, 0.082f, 1.00f);

	colors[ImGuiCol_FrameBgHovered] =
		ImVec4(0.120f, 0.140f, 0.165f, 1.00f);

	colors[ImGuiCol_FrameBgActive] =
		ImVec4(0.150f, 0.175f, 0.205f, 1.00f);

	// Title
	colors[ImGuiCol_TitleBg] =
		ImVec4(0.055f, 0.063f, 0.075f, 1.00f);

	colors[ImGuiCol_TitleBgActive] =
		ImVec4(0.075f, 0.086f, 0.102f, 1.00f);

	colors[ImGuiCol_TitleBgCollapsed] =
		ImVec4(0.055f, 0.063f, 0.075f, 1.00f);

	// Button
	colors[ImGuiCol_Button] =
		ImVec4(0.106f, 0.122f, 0.145f, 1.00f);

	colors[ImGuiCol_ButtonHovered] =
		ImVec4(0.145f, 0.169f, 0.200f, 1.00f);

	colors[ImGuiCol_ButtonActive] =
		ImVec4(0.173f, 0.200f, 0.239f, 1.00f);

	// Header
	colors[ImGuiCol_Header] =
		ImVec4(0.105f, 0.125f, 0.150f, 1.00f);

	colors[ImGuiCol_HeaderHovered] =
		ImVec4(0.145f, 0.175f, 0.210f, 1.00f);

	colors[ImGuiCol_HeaderActive] =
		ImVec4(0.180f, 0.215f, 0.260f, 1.00f);

	// Tab
	colors[ImGuiCol_Tab] =
		ImVec4(0.063f, 0.075f, 0.094f, 1.00f);

	colors[ImGuiCol_TabHovered] =
		ImVec4(0.135f, 0.160f, 0.190f, 1.00f);

	colors[ImGuiCol_TabActive] =
		ImVec4(0.106f, 0.125f, 0.153f, 1.00f);

	colors[ImGuiCol_TabUnfocused] =
		ImVec4(0.047f, 0.055f, 0.067f, 1.00f);

	colors[ImGuiCol_TabUnfocusedActive] =
		ImVec4(0.080f, 0.094f, 0.114f, 1.00f);

	// Separator
	colors[ImGuiCol_Separator] =
		ImVec4(0.160f, 0.180f, 0.205f, 1.00f);

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] =
		ImVec4(0.045f, 0.052f, 0.063f, 1.00f);

	colors[ImGuiCol_ScrollbarGrab] =
		ImVec4(0.160f, 0.180f, 0.205f, 1.00f);

	colors[ImGuiCol_ScrollbarGrabHovered] =
		ImVec4(0.230f, 0.255f, 0.290f, 1.00f);

	colors[ImGuiCol_ScrollbarGrabActive] =
		ImVec4(0.290f, 0.320f, 0.360f, 1.00f);
#endif
}
