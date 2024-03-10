#include "pch.h"
#include "Mesh.h"

#include "MintRenderer.h"

using namespace MintChoco;

D3D12_INPUT_ELEMENT_DESC MintChoco::sVertex::InputElement[] = {
	{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"TexCoord", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"Joint", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{"Weight", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

cPrimitive::cPrimitive() {
	VertexBufferView = {};
	IndexBufferView = {};
}

cPrimitive::~cPrimitive() {
	Destroy();
}

bool cPrimitive::Create(const cMintRenderer& Renderer, const std::vector<sVertex>& Vertices, D3D_PRIMITIVE_TOPOLOGY _Topology) {
	std::vector<UINT> Indecies;
	return Create(Renderer, Vertices, Indecies, _Topology);
}

bool cPrimitive::Create(const cMintRenderer& Renderer, const std::vector<sVertex>& Vertices, const std::vector<UINT>& Indecies, D3D_PRIMITIVE_TOPOLOGY _Topology) {
	if (!Renderer.GetDevice().IsCreated())
		return false;

	if (IsCreated())
		return false;

	if (Vertices.size() == 0)
		return false;

	const size_t VertexBufferSize = sizeof(sVertex) * Vertices.size();


	cInstantCommandQueue Queue;
	if (!Renderer.CreateInstantCommandQueue(Queue, cDevice::eCommandType::COMMAND_TYPE_DIRECT))
		return false;

	cResourceFactory ResourceFactory(Renderer.GetDevice());

	cResource VertexUploadBuffer;
	bool bResult = ResourceFactory.CreateUploadBuffer(VertexUploadBuffer, VertexBufferSize);
	if (!bResult)
		return false;
	bResult = ResourceFactory.CreateBuffer(VertexBuffer, VertexBufferSize, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
	if (!bResult)
		return false;

	auto UploadToken = VertexUploadBuffer.Map(0, VertexBufferSize);
	if (!UploadToken.Ptr)
		return false;
	memcpy(UploadToken.Ptr, Vertices.data(), VertexBufferSize);
	VertexUploadBuffer.Unmap(UploadToken);

	Queue.GetList().ResourceTransition(VertexBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	Queue.GetList().CopyResource(VertexBuffer, VertexUploadBuffer);
	Queue.GetList().ResourceTransition(VertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	if (Indecies.size() == 0) {
		Queue.Execute();
		return true;
	}

	const size_t IndexBufferSize = sizeof(UINT) * Indecies.size();
	cResource IndexUploadBuffer;
	bResult = ResourceFactory.CreateUploadBuffer(IndexUploadBuffer, IndexBufferSize);
	if (!bResult)
		return false;
	bResult = ResourceFactory.CreateBuffer(IndexBuffer, IndexBufferSize, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
	if (!bResult)
		return false;

	UploadToken = IndexUploadBuffer.Map(0, IndexBufferSize);
	if (!UploadToken.Ptr)
		return false;
	memcpy(UploadToken.Ptr, Indecies.data(), IndexBufferSize);
	IndexUploadBuffer.Unmap(UploadToken);

	Queue.GetList().ResourceTransition(IndexBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	Queue.GetList().CopyResource(IndexBuffer, IndexUploadBuffer);
	Queue.GetList().ResourceTransition(IndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	Queue.Execute();
	Queue.Destroy();

	VertexBufferView.BufferLocation = VertexBuffer.GetResouce()->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes	= sizeof(sVertex);
	VertexBufferView.SizeInBytes	= VertexBufferSize;
	if (IndexBuffer.IsCreated()) {
		IndexBufferView.BufferLocation	= IndexBuffer.GetResouce()->GetGPUVirtualAddress();
		IndexBufferView.Format			= DXGI_FORMAT_R32_UINT;
		IndexBufferView.SizeInBytes		= IndexBufferSize;
	}

	Topology = _Topology;

	return true;
}

void cPrimitive::Destroy() {
	Vertices.clear();
	Indecies.clear();

	VertexBuffer.Destroy();
	IndexBuffer.Destroy();

	VertexBufferView = {};
	IndexBufferView = {};
}
