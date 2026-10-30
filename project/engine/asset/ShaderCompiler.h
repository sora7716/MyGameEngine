#pragma once
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <string>
#include <dxcapi.h>
#include <wrl.h>
#include <memory>

/// <summary>
/// シェーダコンパイラ
/// </summary>
class ShaderCompiler{
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
	/// <returns>コンパイラシェーダー</returns>
	ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

	/// <summary>
	/// シェーダリフレクション
	/// </summary>
	/// <param name="shaderResult">コンパイルしたシェーダの結果</param>
	void ReflectionShader(ComPtr<IDxcResult> shaderResult);
private://メンバ関数
	/// <summary>
	/// DXCコンパイラの生成
	/// </summary>
	void CreateDXCCompiler();
private://メンバ変数
	//DXCユーティリティ
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	//DXCコンパイラ
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	//デフォルトインクルードハンドラ
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
};

