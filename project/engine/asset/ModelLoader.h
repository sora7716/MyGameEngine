#pragma once
#include "RenderData.h"

namespace modelLoader{

	/// <summary>
	/// .mtlファイルの読み取り	
	/// </summary>
	/// <param name="directoryPath">ディレクトリファイルパス</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>マテリアルデータ</returns>
	MaterialTexturePaths LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="directoryPath">ディレクトリファイルパス(最後に"/"はいらない)</param>
	/// <param name="fileName">ファイル名</param>
	/// <returns>モデルデータ</returns>
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& fileName);
}

