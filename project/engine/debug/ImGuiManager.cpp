#include "ImGuiManager.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "WinApi.h"

//デストラクタ
ImGuiManager::~ImGuiManager() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // USE_IMGUI
}

//初期化
void ImGuiManager::Initialize(WinApi* winApi, DirectXBase* directXBase, SRVManager* srvManager) {
#ifdef USE_IMGUI
	//DirectXの基盤部分を記録する
	directXBase_ = directXBase;
	//SRVの管理を記録する
	srvManager_ = srvManager;
	IMGUI_CHECKVERSION();
	//ImGuiのコンテキストを生成
	ImGui::CreateContext();
	//ImGuiのスタイルを設定
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApi->GetHwnd());
	//srvの確保
	srvManager_->Allocate();
	ImGui_ImplDX12_Init(
		directXBase_->GetDevice(),
		static_cast<int>(directXBase_->GetSwapChainResourceNum()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvManager_->GetDescriptorHeap(),
		srvManager_->GetCPUDescriptorHandle(0),
		srvManager_->GetGPUDescriptorHandle(0));
	//srvの解放
	srvManager_->Free(0);
#endif // USE_IMGUI
}

//ImGuiの受付開始
void ImGuiManager::Begin() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // USE_IMGUI
}

//ImGuiの受付終了
void ImGuiManager::End() {
#ifdef USE_IMGUI
	//描画前準備
	ImGui::Render();
#endif // USE_IMGUI
}

//描画
void ImGuiManager::Draw() {
#ifdef USE_IMGUI
	ID3D12GraphicsCommandList* commandList = directXBase_->GetCommandList();
	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvManager_->GetDescriptorHeap() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), directXBase_->GetCommandList());
#endif // USE_IMGUI
}

//トランスフォームデータ用のImGui
void ImGuiManager::DragTransform(TransformData& transfromData) {
#ifdef USE_IMGUI
	ImGui::DragFloat3("scale", &transfromData.scale.x, 0.1f);
	ImGui::DragFloat3("rotate", &transfromData.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &transfromData.translate.x, 0.1f);
#endif // USE_IMGUI
}

//int型でcheckBoxを表示する
void ImGuiManager::CheckBoxToInt(const std::string& label, int32_t& frag) {
	bool checkBox = static_cast<bool>(frag);
#ifdef USE_IMGUI
	ImGui::Checkbox(label.c_str(), &checkBox);
#endif // USE_IMGUI
	frag = static_cast<int32_t>(checkBox);
}

//4x4の行列を表示する
void ImGuiManager::ScreenMatrix4x4(const Matrix4x4& matrix, const char* label) {
	ImGui::SeparatorText(label);
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {

			ImGui::Text("%5.3f", matrix.m[i][j]);

			if (j < 3) {
				ImGui::SameLine();
			}
		}
	}
}

//クオータニオンの表示
void ImGuiManager::ScreenQuaternion(const Quaternion& quaternion, const char* label) {
	ImGui::SeparatorText(label);
	ImGui::Text("%5.3f,%5.3f,%5.3f,%5.3f", quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

//浮動小数の表示
void ImGuiManager::ScreenFloat(float num, const char* label) {
	ImGui::SeparatorText(label);
	ImGui::Text("%5.3f", num);
}

//コンストラクタ
ImGuiManager::ImGuiManager(ConstructorKey) {
}
