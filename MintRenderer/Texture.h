#pragma once
#include <glm/vec2.hpp>

#include <d3d12.h>

#include "Resource.h"
#include "Image.h"

namespace MintChoco {
	class cMintRenderer;

	class cTexture {
public:
	cTexture();
	~cTexture();

	bool CreateTex2D(cMintRenderer& Renderer, const glm::uvec2& Size, DXGI_FORMAT Format);
	bool CreateFromImage(cMintRenderer& Renderer, const cImage& Image);

	void Destroy();

	bool CopyImage(cMintRenderer& Renderer, const cImage& Image);

	bool IsCreated() const { return TextureBuffer.IsCreated(); }
	cResource& GetBuffer() { return TextureBuffer; }
	const cResource& GetBuffer() const { return TextureBuffer; }
protected:
	cResource TextureBuffer;
};
}

