#pragma once
#include "RenderData.h"

/// <summary>
/// プリミティブのメッシュを作成
/// </summary>
namespace PrimitiveMeshFactory {
	/// <summary>
	/// 立方体メッシュの作成
	/// </summary>
	/// <returns>立方体メッシュ</returns>
	MeshData CreateCube();

	/// <summary>
	/// 球メッシュの作成
	/// </summary>
	/// <returns>球メッシュ</returns>
	MeshData CreateSphere();

	/// <summary>
	/// 平面メッシュの作成
	/// </summary>
	/// <returns>平面メッシュ</returns>
	MeshData CreatePlane();
}

