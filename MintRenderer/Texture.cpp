#include "pch.h"
#include "Texture.h"

#include "MintRenderer.h"

using namespace MintChoco;

cTexture::cTexture() {

}

cTexture::~cTexture() {
	Destroy();
}

bool cTexture::CreateTex2D(cMintRenderer& Renderer, const glm::uvec2& Size, DXGI_FORMAT Format) {
	if (IsCreated())
		return false;

	cResourceFactory Factory(Renderer.GetDevice());
	if (!Factory.CreateTextureBuffer(TextureBuffer, cVolumeui(Size.x, Size.y, 0), false, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_NONE))
		return false;

	return true;
}

bool cTexture::CreateFromImage(cMintRenderer& Renderer, const cImage& Image) {
	if (!Image.IsCreated())
		return false;

	const DXGI_FORMAT Format = GetDXGIFormat(Image.GetPixelFormat());
	const glm::uvec2 Size(Image.GetImageSize().Width, Image.GetImageSize().Height);

	if (!CreateTex2D(Renderer, Size, Format))
		return false;

	return CopyImage(Renderer, Image);
}

void cTexture::Destroy() {
	TextureBuffer.Destroy();
}

bool cTexture::CopyImage(cMintRenderer& Renderer, const cImage& Image) {
	if (!Image.IsCreated())
		return false;

	if (!IsCreated())
		return false;

	if (TextureBuffer.GetDescription1().Format != GetDXGIFormat(Image.GetPixelFormat()))
		return false;

	if (TextureBuffer.GetDescription1().DepthOrArraySize != 1)
		return false;

	if (TextureBuffer.GetDescription1().Width  != Image.GetImageSize().Width ||
		TextureBuffer.GetDescription1().Height != Image.GetImageSize().Height )
		return false;

	const DXGI_FORMAT DXGIFormat = GetDXGIFormat(Image.GetPixelFormat());
	const size_t AlignedTexturePitch = ((Image.GetStride() + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1) / D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
	const size_t UploadBufferSize = AlignedTexturePitch * Image.GetImageSize().Height;
	const size_t RowCount = Image.GetImageSize().Height;
	cResource UploadBuffer;

	cResourceFactory Factory(Renderer.GetDevice());
	if (!Factory.CreateUploadBuffer(UploadBuffer, UploadBufferSize))
		return false;

	auto Token = UploadBuffer.Map(0, UploadBufferSize);
	if (!Token.Ptr)
		return false;
	for (size_t iRow = 0; iRow < RowCount; ++iRow) {
		memcpy((BYTE*)Token.Ptr + AlignedTexturePitch * iRow, Image.GetData() + Image.GetStride() * iRow, Image.GetStride());
	}
	UploadBuffer.Unmap(Token);

	D3D12_TEXTURE_COPY_LOCATION SrcLocation, DstLocation;
	SrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	SrcLocation.pResource = UploadBuffer.GetResouce();
	SrcLocation.PlacedFootprint.Offset = 0;
	SrcLocation.PlacedFootprint.Footprint.Format = DXGIFormat;
	SrcLocation.PlacedFootprint.Footprint.Width = Image.GetImageSize().Width;
	SrcLocation.PlacedFootprint.Footprint.Height= Image.GetImageSize().Height;
	SrcLocation.PlacedFootprint.Footprint.Depth = 1;
	SrcLocation.PlacedFootprint.Footprint.RowPitch = AlignedTexturePitch;

	DstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	DstLocation.pResource = TextureBuffer.GetResouce();
	DstLocation.SubresourceIndex = 0;

	D3D12_BOX Box = {};
	Box.left = Box.top = Box.front = 0;
	Box.right = Image.GetImageSize().Width;
	Box.bottom = Image.GetImageSize().Height;
	Box.back = 1;

	cInstantCommandQueue Queue;
	Renderer.CreateInstantCommandQueue(Queue, cDevice::eCommandType::COMMAND_TYPE_DIRECT);
	if (!Queue.IsCreated())
		return false;

	Queue.GetList().ResourceTransition(TextureBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	Queue.GetList().GetGraphicsCommandList()->CopyTextureRegion(&DstLocation, 0, 0, 0, &SrcLocation, &Box);
	Queue.GetList().ResourceTransition(TextureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
	Queue.Execute();
	Queue.Destroy();

	return true;
}
