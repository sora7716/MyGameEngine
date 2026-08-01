#pragma once
#include <vector>

//前方宣言
class Camera;
class Model;

struct LODRenderData{
	std::vector<Model*> models;
	std::vector<uint32_t>srvIndices = {};
	std::vector<uint32_t>drawCounts = {};

	/// <summary>
	/// サイズがあっている確認
	/// </summary>
	/// <returns>サイズがあっているか</returns>
	bool IsLodCountValid()const;
};

//描画に必要なデータ
struct Object3dRenderData{
	Camera* renderCamera = nullptr;
	LODRenderData lodRenderData;
};