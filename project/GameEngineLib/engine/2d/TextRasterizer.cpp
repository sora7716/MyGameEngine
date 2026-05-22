#include "TextRasterizer.h"
#include <cassert>

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"windowscodecs.lib")

using Microsoft::WRL::ComPtr;

//初期化
void TextRasterizer::Initialize() {
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_));
	assert(SUCCEEDED(hr));

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&d2dFactory_));
	assert(SUCCEEDED(hr));

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dwrite_.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

CpuBitmap TextRasterizer::RenderTextToCpuBitmap(const std::wstring& text, uint32_t width, uint32_t height, const std::wstring& fontName, float textSize, const Vector4& color) {
	CpuBitmap out;
	out.width = width;
	out.height = height;

	//WIC bitmap(32bppPBGRA)
	ComPtr<IWICBitmap>wicBitmap;
	HRESULT hr = wic_->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &wicBitmap);
	assert(SUCCEEDED(hr));

	//D2D render target をWIC bitmap に紐づけ
	ComPtr<ID2D1RenderTarget>renderTarget;
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0, 0);
	hr = d2dFactory_->CreateWicBitmapRenderTarget(wicBitmap.Get(), props, &renderTarget);
	assert(SUCCEEDED(hr));

	//フォント設定
	ComPtr<IDWriteTextFormat>format;
	hr = dwrite_->CreateTextFormat(
		fontName.c_str(), nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		textSize,
		L"ja-jp",
		&format);
	assert(SUCCEEDED(hr));
	format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	//ブラシ(白)
	ComPtr<ID2D1SolidColorBrush> brush;
	FLOAT r = color.x;
	FLOAT g = color.y;
	FLOAT b = color.z;
	FLOAT a = color.w;
	hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(r, g, b, a), &brush);
	assert(SUCCEEDED(hr));

	//描画
	renderTarget->BeginDraw();
	renderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));

	D2D1_RECT_F rect = D2D1::RectF(0, 0, float(width), float(height));
	renderTarget->DrawTextW(text.c_str(), static_cast<UINT32>(text.size()), format.Get(), rect, brush.Get());
	hr = renderTarget->EndDraw();
	assert(SUCCEEDED(hr));

	//WIC bitmap からピクセルを取り出し(stride対応)
	WICRect rc{ 0,0,(INT)width,(INT)height };
	ComPtr<IWICBitmapLock>lock;
	hr = wicBitmap->Lock(&rc, WICBitmapLockRead, &lock);
	assert(SUCCEEDED(hr));

	UINT bufferSize = 0;
	BYTE* data = nullptr;
	hr = lock->GetDataPointer(&bufferSize, &data);
	assert(SUCCEEDED(hr));

	UINT stride = 0;
	hr = lock->GetStride(&stride);
	assert(SUCCEEDED(hr));

	out.stride = stride;
	out.bgra.resize(size_t(stride) * height);

	memcpy(out.bgra.data(), data, bufferSize);

	return out;
}
