#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>

//シェーダのタイプ
enum class ShaderType :uint32_t{
	kAll = D3D12_SHADER_VISIBILITY_ALL,
	kPixel = D3D12_SHADER_VISIBILITY_PIXEL,
	kVertex = D3D12_SHADER_VISIBILITY_VERTEX
};

/// <summary>
/// ルートシグネイチャの構築
/// </summary>
class RootSignatureBuilder{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RootSignatureBuilder();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RootSignatureBuilder();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 構築
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <returns>ルートシグネイチャ</returns>
	ComPtr<ID3D12RootSignature> Build(ID3D12Device* device);

	/// <summary>
	/// コンスタントバッファビューの追加
	/// </summary>
	/// <param name="shaderType">シェーダタイプ</param>
	/// <param name="registerNumber">レジスター番号</param>
	void AddCBV(ShaderType shaderType, uint32_t registerNumber);

	/// <summary>
	/// ストラクチャバッファテーブルの追加
	/// </summary>
	/// <param name="shaderType">シェーダタイプ</param>
	/// <param name="registerNumber">レジスター番号</param>
	void AddSRVTable(ShaderType shaderType, uint32_t registerNumber);

	/// <summary>
	/// サンプラーの追加
	/// </summary>
	/// <param name="shaderType">シェーダタイプ</param>
	/// <param name="registerNumber">レジスター番号</param>
	void AddStaticSampler(ShaderType shaderType, uint32_t registerNumber);
private://メンバ変数
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	//Samplerの設定
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;
	//DescriptorRange
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges_;
	//RootParameterの作成。複数設定できるので配列。
	std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
};

