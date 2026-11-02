#pragma once
#include "ShaderInfo.h"
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <d3d12shader.h>
#include <wrl.h>
#include <string>
#include <memory>
#include <vector>

/// <summary>
/// シェーダコンパイラ
/// </summary>
class ShaderCompiler {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<ShaderCompiler>Create();
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ShaderCompiler();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ShaderCompiler();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// シェーダーのコンパイラ
	/// </summary>
	/// <param name="filePath">CompilerするShaderファイルへのパス</param>
	/// <param name="profile">Compilerに使用するProfile</param>
	void CompileShader(const std::wstring& filePath , const wchar_t* profile);

	/// <summary>
	/// シェーダの情報を取得
	/// </summary>
	/// <returns>シェーダの情報</returns>
	const ShaderInfo& GetShaderInfo()const;
private://メンバ関数
	/// <summary>
	/// DXCコンパイラの生成
	/// </summary>
	void CreateDXCCompiler();

	/// <summary>
	/// シェーダリフレクション
	/// </summary>
	/// <param name="shaderResult">コンパイルしたシェーダの結果</param>
	void ReflectionShader(ComPtr<IDxcResult> shaderResult);

	/// <summary>
	/// パラメータの型をDirectXの型からわかりやすいように変換
	/// </summary>
	/// <param name="desc">DirectXでの型を取得</param>
	/// <returns>わかりやすい型に変換</returns>
	ParameterType ConvertToParameterType(const D3D12_SHADER_TYPE_DESC& desc);
private://メンバ変数
	//DXCユーティリティ
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	//DXCコンパイラ
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	//デフォルトインクルードハンドラ
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	//シェーダの情報
	ShaderInfo shaderInfo_ = {};
};

