#pragma once
#include <wrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <cstdint>
#include <vector>
#include <string>
#include "Vector4.h"

//CPUビットマップ
struct CpuBitmap {
	uint32_t width;
	uint32_t height;
	uint32_t stride;//1行のバイト数
	std::vector<uint8_t>bgra;//BGRA 4byte/pixel
};

/// <summary>
/// 文字のラスタライザ
/// </summary>
class TextRasterizer {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TextRasterizer() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TextRasterizer() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// フォントの作成
	/// </summary>
	/// <param name="text">入力したい文字</param>
	/// <param name="width">文字の描画範囲(横幅)</param>
	/// <param name="height">文字の描画範囲(縦幅)</param>
	/// <param name="fontName">使用するfontのファミリー名</param>
	/// <param name="fontSize">文字のサイズ</param>
	/// <param name="color">文字の色</param>
	/// <returns></returns>
	CpuBitmap RenderTextToCpuBitmap(const std::wstring& text, uint32_t width, uint32_t height, const std::wstring& fontName = L"Yu Gothic UI", float textSize = 32.0f,const Vector4& color={1.0f,1.0f,1.0f,1.0f});
private://メンバ変数
	ComPtr<IWICImagingFactory>wic_;
	ComPtr<ID2D1Factory>d2dFactory_;
	ComPtr<IDWriteFactory>dwrite_;
};

