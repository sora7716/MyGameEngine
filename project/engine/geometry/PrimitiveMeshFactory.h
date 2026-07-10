#pragma once
#include "Vector3.h"
#include "RenderData.h"

/// <summary>
/// プリミティブのメッシュを作成
/// </summary>
namespace primitiveMeshFactory {
	//設定データ
	struct Desc {
		Vector3 size = Vector3::MakeAllOne();
		uint32_t subdivision = 16;
		float radius = 1.0f;
	};

	/// <summary>
	/// 立方体メッシュの作成
	/// </summary>
	/// <returns>立方体メッシュ</returns>
	/// <param name="desc">設定データ</param>
	/// <returns></returns>
	MeshData CreateCube(const Desc& desc = {});

	/// <summary>
	/// 球メッシュの作成
	/// </summary>
	/// <param name="desc">設定データ</param>
	/// <returns>球メッシュ</returns>
	MeshData CreateSphere(const Desc& desc = {});

	/// <summary>
	/// 平面メッシュの作成
	/// </summary>
	/// <param name="desc">設定データ</param>
	/// <returns>平面メッシュ</returns>
	MeshData CreatePlane(const Desc& desc = {});
}

