#include "pch.h"
#include "Image.h"

#include <algorithm>
#include <mutex>
#include <array>
#include <cassert>
#include <wincodec.h>

using namespace MintChoco;

std::array<sPixelFormatTableEntry, (size_t)ePixelFormat::PIXEL_FORMAT_COUNT + 1> PixelFormatTable = {
	sPixelFormatTableEntry{ePixelFormat::PIXEL_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, 32, 4, 4},
	sPixelFormatTableEntry{ePixelFormat::PIXEL_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM, 32, 4, 4},

	sPixelFormatTableEntry{ePixelFormat::PIXEL_FORMAT_COUNT, DXGI_FORMAT_UNKNOWN,0,0,0},
};

const sPixelFormatTableEntry& MintChoco::GetPixelFormatTable(ePixelFormat Format) {
	return PixelFormatTable[static_cast<int>(Format)];
}

const sPixelFormatTableEntry& MintChoco::GetPixelFormatTable(DXGI_FORMAT Format) {
	for (const sPixelFormatTableEntry& Entry : PixelFormatTable)
		if (Format == Entry.DxgiFormat)
			return Entry;

	return PixelFormatTable.back();
}

DXGI_FORMAT MintChoco::GetDXGIFormat(ePixelFormat Format) {
	return GetPixelFormatTable(Format).DxgiFormat;
}

ePixelFormat MintChoco::GetPixelFormat(DXGI_FORMAT Format) {
	return GetPixelFormatTable(Format).PixelFormat;
}

size_t MintChoco::GetPixelSize(DXGI_FORMAT Format) {
	switch(Format) {
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 16;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 12;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 8;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		return 4;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
		return 2;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 1;
	default:
		assert(false);
	}
	return 0;
}

bool cImageLoader::Load(cImage& Image, LPCTSTR ImagePath) {
	ComPtr<IWICImagingFactory> FactoryPtr;

	auto Result = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(FactoryPtr.ReleaseAndGetAddressOf())
	);

	if (Result != S_OK)
		return false;

	ComPtr<IWICBitmapDecoder> DecoderPtr;
	Result = FactoryPtr->CreateDecoderFromFilename(
		ImagePath,
		NULL,                            
		GENERIC_READ,                    
		WICDecodeMetadataCacheOnDemand,
		DecoderPtr.ReleaseAndGetAddressOf()
	);

	if (Result != S_OK)
		return false;

	ComPtr<IWICBitmapFrameDecode> FrameDecoderPtr;
	Result = DecoderPtr->GetFrame(0, FrameDecoderPtr.ReleaseAndGetAddressOf());

	if (Result != S_OK)
		return false;

	WICPixelFormatGUID WICPixelFormat;
	Result = FrameDecoderPtr->GetPixelFormat(&WICPixelFormat);

	if (Result != S_OK)
		return false;

	ComPtr<IWICFormatConverter> ConverterPtr;
	Result = FactoryPtr->CreateFormatConverter(ConverterPtr.ReleaseAndGetAddressOf());

	if (Result != S_OK)
		return false;

	Result = ConverterPtr->Initialize(
		FrameDecoderPtr.Get(),
		GUID_WICPixelFormat32bppBGRA,
		WICBitmapDitherTypeNone,
		NULL, 0.f, WICBitmapPaletteTypeMedianCut);

	if (Result != S_OK)
		return false;

	cSizei ImageSize;
	UINT Width, Height;
	Result = ConverterPtr->GetSize(&Width, &Height);

	if (Result != S_OK)
		return false;

	ImageSize.Width	 = Width;
	ImageSize.Height = Height;
	Image.Create(ImageSize, ePixelFormat::PIXEL_FORMAT_B8G8R8A8_UNORM);

	Result = ConverterPtr->CopyPixels(
		NULL,
		Image.GetStride(),
		Image.GetDataSize(),
		Image.GetData()
	);

	if (Result != S_OK) {
		Image.Destroy();
		return false;
	}

	return true;
}

cImage::cImage() {
	Size = cSizei();
	PixelFormat = ePixelFormat::PIXEL_FORMAT_COUNT;
	BPP = 0;
	Stride = 0;
	DataSize = 0;
	pData = nullptr;
}

cImage::~cImage() {
	Destroy();
}

bool cImage::Create(cSizei _Size, ePixelFormat _Format) {
	Size = _Size;
	PixelFormat = _Format;

	const sPixelFormatTableEntry& PixelFormatTableEntry = GetPixelFormatTable(PixelFormat);
	BPP = PixelFormatTableEntry.BitsPP;
	Stride = ((Size.Width * ((BPP + 7) / 8) + Alignment - 1) / Alignment) * Alignment;
	DataSize = Stride * Size.Height;
	pData = new BYTE[DataSize];

	if (pData == nullptr)
		return false;

	return true;
}

void cImage::Destroy() {
	if (pData)
		delete[] pData;

	Size = cSizei();
	PixelFormat = ePixelFormat::PIXEL_FORMAT_COUNT;
	BPP = 0;
	Stride = 0;
	DataSize = 0;
	pData = 0;
}