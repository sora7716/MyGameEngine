#pragma once
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>
#include <memory>
#include <vector>

//コンパイルしたときのシェーダの情報
struct CompileShaderInfo{
	std::string name;
	uint32_t offset;
	uint32_t size;
};

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
	/// コンパイル済みのシェーダBlobを取得
	/// </summary>
	/// <returns>コンパイル済みのシェーダBlob</returns>
	const ComPtr<IDxcBlob>& GetShaderBlob()const;

	/// <summary>
	/// コンパイルした際のシェーダ情報を取得
	/// </summary>
	/// <returns>コンパイルした際のシェーダ情報</returns>
	const std::vector<CompileShaderInfo>& GetCompileShaderInfos()const;

	/// <summary>
	/// コンパイルしたシェーダの情報の数の取得
	/// </summary>
	/// <returns>コンパイルしたシェーダの情報の数</returns>
	uint32_t GetCompileShaderInfoCount()const;
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
private://メンバ変数
	//DXCユーティリティ
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	//DXCコンパイラ
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	//デフォルトインクルードハンドラ
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	//コンパイルしたシェーダの情報
	ComPtr<IDxcBlob> shaderBlob_ = nullptr;
	//コンパイルしたときのシェーダの情報
	std::vector<CompileShaderInfo>compileShaderInfos_;
};

