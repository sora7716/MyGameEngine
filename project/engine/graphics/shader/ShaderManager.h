#pragma once
#include "ShaderInfo.h"
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <memory>
#include <string>
#include <unordered_map>

//前方宣言
class ShaderCompiler;

/// <summary>
/// シェーダの管理
/// </summary>
class ShaderManager{
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
	/// <param name="key">キー</param>
	/// <returns></returns>
	static std::unique_ptr<ShaderManager>Create(ConstructorKey key);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit ShaderManager(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ShaderManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// シェーダの情報を取得
	/// </summary>
	/// <param name="name">登録したシェーダ名</param>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="profile">プロファイル</param>
	/// <returns>シェーダ</returns>
	ShaderInfo* FindShaderInfo(const std::string& name, const std::string& filePath, const wchar_t* profile);
private://メンバ変数
	//シェーダの情報を格納する配列
	std::unordered_map<std::string, ShaderInfo> shaderInfos_;
	//シェーダコンパイラ
	std::unique_ptr<ShaderCompiler>shaderCompiler_ = nullptr;
};

