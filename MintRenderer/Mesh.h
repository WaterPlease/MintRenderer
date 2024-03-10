#pragma once

#include <string>
#include <vector>
#include <Windows.h>

#include "glm/glm.hpp"
#include <d3d12.h>

#include "Resource.h"

namespace MintChoco {
#pragma pack(push, 4)
	struct sVertex {
		glm::vec3		Position;
		glm::vec3		Normal;
		glm::vec3		Tangent;
		glm::vec2		TexCoord0;
		glm::vec2		TexCoord1;
		glm::vec4		Color0;
		glm::uvec4		Joint0;
		glm::vec4		Weight0;

		static D3D12_INPUT_ELEMENT_DESC InputElement[8];
		static constexpr size_t GetAttributeCount() { return sizeof(InputElement) / sizeof(D3D12_INPUT_ELEMENT_DESC); }
	};
#pragma pack(pop)
	class cMintRenderer;
	class cPrimitive {
	public:
		cPrimitive();
		~cPrimitive();
		
		bool Create(const cMintRenderer& Device, const std::vector<sVertex>& Vertices, D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		bool Create(const cMintRenderer& Device, const std::vector<sVertex>& Vertices, const std::vector<UINT>& Indecies, D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void Destroy();

		bool IsCreated() const { return VertexBuffer.IsCreated(); }
		bool IsUsingIndex() const { return IndexBuffer.IsCreated(); }

		D3D_PRIMITIVE_TOPOLOGY			GetTopology()			const { return Topology; }
		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView()	const { return VertexBufferView; }
		const D3D12_INDEX_BUFFER_VIEW&	GetIndexBufferView()	const { return IndexBufferView;  }
	protected:
		std::vector<sVertex>	Vertices;
		std::vector<UINT>		Indecies;

		cResource				VertexBuffer;
		cResource				IndexBuffer;

		D3D_PRIMITIVE_TOPOLOGY		Topology;
		D3D12_VERTEX_BUFFER_VIEW	VertexBufferView;
		D3D12_INDEX_BUFFER_VIEW		IndexBufferView;
	};
}

