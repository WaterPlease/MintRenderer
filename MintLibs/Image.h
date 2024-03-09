#pragma once

#include "Geometry.h"
#include <d3d12.h>
#include <wrl.h>

namespace MintChoco {

	using Microsoft::WRL::ComPtr;

enum class ePixelFormat {
	PIXEL_FORMAT_R8G8B8A8_UNORM,
	PIXEL_FORMAT_B8G8R8A8_UNORM,

	PIXEL_FORMAT_COUNT,
};

struct sPixelFormatTableEntry {
	ePixelFormat	PixelFormat;
	DXGI_FORMAT		DxgiFormat;
	UINT			BitsPP;
	UINT			BytePP;
	UINT			nChannel;
};
const sPixelFormatTableEntry& GetPixelFormatTable(ePixelFormat);
const sPixelFormatTableEntry& GetPixelFormatTable(DXGI_FORMAT);
DXGI_FORMAT	GetDXGIFormat(ePixelFormat);
ePixelFormat	GetPixelFormat(DXGI_FORMAT);
size_t			GetPixelSize(DXGI_FORMAT);

class cImage;
class cImageLoader {
public:
	static bool Load(cImage& Image, LPCTSTR ImagePath);
};

class cImage {
public:
	const size_t Alignment = 4;

	cImage();
	~cImage();

	bool Create(cSizei Size, ePixelFormat Format);
	void Destroy();

	const cSizei&	GetImageSize() { return Size;}

	ePixelFormat GetPixelFormat() const { return PixelFormat; }
	UINT GetBPP() const { return BPP; }
	UINT GetStride() const { return Stride; }
	size_t	GetDataSize() const { return DataSize; }

	BYTE*	GetData() { return pData; }
	const BYTE*	GetData() const { return pData; }
protected:
	cSizei			Size;
	ePixelFormat	PixelFormat;
	UINT			BPP;
	UINT			Stride;

	size_t			DataSize;
	BYTE*			pData;
};

}
