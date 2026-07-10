#include "ImGuiManager.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "MatrixUtility.h"
#include "MathUtility.h"
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
	ImGui_ImplWin32_Init(winApi->GetHwnd(0));
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
void ImGuiManager::DragTransform(Transform& transformData) {
#ifdef USE_IMGUI
	ImGui::Checkbox("isUsingQuaternion", &transformData.isUsingQuaternion);
	ImGui::DragFloat3("scale", &transformData.scale.x, 0.1f);
	if (transformData.isUsingQuaternion) {
		ImGui::DragFloat3("axis", &transformData.axis.x, 0.01f, -1.0f, 1.0f);
		ImGui::SliderAngle("angle", &transformData.angle);
		transformData.quaternion = matrixUtility::MakeRotateAxisAngleQuaternion(transformData.axis, transformData.angle);
		transformData.eulerAngle = mathUtility::MakeEulerAngleForQuaternion(transformData.quaternion);
	} else {
		ImGui::DragFloat3("eulerAngle", &transformData.eulerAngle.x, 0.1f);
		transformData.quaternion = Quaternion::MakeQuaternionForEulerAngle(transformData.eulerAngle);
	}
	ImGui::DragFloat4("rotate", &transformData.quaternion.x, 0.0f);
	ImGui::DragFloat3("translate", &transformData.translate.x, 0.01f);
#endif // USE_IMGUI
}

//OBBデータ用のImGui
void ImGuiManager::DragOBB(primitiveData::OBB& obb) {
#ifdef USE_IMGUI
	ImGui::DragFloat3("size", &obb.size.x, 0.1f);
	static Vector3 obbRadian = {};
	ImGui::DragFloat3("rotate", &obbRadian.x, 0.1f);
	obb.quaternion = Quaternion::MakeQuaternionForEulerAngle(obbRadian);
	ImGui::DragFloat3("center", &obb.center.x, 0.1f);
#endif // USE_IMGUI
}

//円用のImGui
void ImGuiManager::DragCircle(primitiveData::Circle& circle) {
#ifdef USE_IMGUI
	ImGui::DragFloat3("center", &circle.center.x, 0.1f);
	ImGui::DragFloat3("eulerAngle", &circle.eulerAngle.x, 0.1f);
	ImGui::DragFloat("radius", &circle.radius, 0.01f);
#endif // USE_IMGUI
}

//球用のImGui
void ImGuiManager::DragSphere(primitiveData::Sphere& sphere) {
#ifdef USE_IMGUI
	ImGui::DragFloat3("center", &sphere.center.x, 0.1f);
	ImGui::DragFloat("radius", &sphere.radius, 0.01f);
#endif // USE_IMGUI
}

//int型でcheckBoxを表示する
bool ImGuiManager::CheckBoxToInt(const std::string& label, int32_t& frag) {
	bool checkBox = static_cast<bool>(frag);
#ifdef USE_IMGUI
	ImGui::Checkbox(label.c_str(), &checkBox);
#endif // USE_IMGUI
	frag = static_cast<int32_t>(checkBox);
	return checkBox;
}

//4x4の行列の表示
void ImGuiManager::Matrix4x4Text(const Matrix4x4& matrix, const char* label) {
#ifdef USE_IMGUI
	ImGui::SeparatorText(label);
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {

			ImGui::Text("%5.3f", matrix.m[i][j]);

			if (j < 3) {
				ImGui::SameLine();
			}
		}
	}
#endif // USE_IMGUI
}

//3次元ベクトルの表示
void ImGuiManager::Vector3Text(const Vector3& vector, const char* label) {
#ifdef USE_IMGUI
	ImGui::SeparatorText(label);
	ImGui::Text("%5.3f %5.3f %5.3f", vector.x, vector.y, vector.z);
#endif // USE_IMGUI
}

//クオータニオンの表示
void ImGuiManager::QuaternionText(const Quaternion& quaternion, const char* label) {
#ifdef USE_IMGUI
	ImGui::SeparatorText(label);
	ImGui::Text("%5.3f %5.3f %5.3f %5.3f", quaternion.x, quaternion.y, quaternion.z, quaternion.w);
#endif // USE_IMGUI
}

//浮動小数の表示
void ImGuiManager::FloatText(float num, const char* label) {
#ifdef USE_IMGUI
	ImGui::SeparatorText(label);
	ImGui::Text("%5.3f", num);
#endif // USE_IMGUI
}

//AABBの表示
void ImGuiManager::AABBText(const primitiveData::AABB& aabb, const char* label) {
#ifdef _DEBUG
	ImGuiManager::Vector3Text(aabb.min, (static_cast<std::string>(label) + ".min").c_str());
	ImGuiManager::Vector3Text(aabb.max, (static_cast<std::string>(label) + ".max").c_str());
#endif // _DEBUG

}

//コンストラクタ
ImGuiManager::ImGuiManager(ConstructorKey) {}
