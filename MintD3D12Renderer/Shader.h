#pragma once
#include <vector>

#include "Utilities.h"

namespace MintChoco {
	class cShader {
		DELETE_CLASS_COPY(cShader);
	public:
		cShader();
		virtual ~cShader();

		bool Create(const std::wstring& Path);
		void Destroy();
		D3D12_SHADER_BYTECODE GetD3D12ShaderByteCode() const { return { GetShaderCode(), GetByteLength() }; }
		inline const void*	GetShaderCode() const { return ShaderCode.data(); }
		inline size_t		GetByteLength() const { return ShaderCode.size(); }
	protected:
		std::vector<BYTE> ShaderCode;
	};
}
