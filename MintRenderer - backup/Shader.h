#pragma once
#include <vector>

namespace MintChoco {
	class cShader {
	public:
		cShader();
		~cShader();

		bool Create(const std::wstring& Path);
		void Destroy();
		D3D12_SHADER_BYTECODE GetD3D12ShaderByteCode() const { return { GetShaderCode(), GetByteLength() }; }
		inline const void*	GetShaderCode() const { return ShaderCode.data(); }
		inline size_t		GetByteLength() const { return ShaderCode.size(); }
	protected:
		std::vector<BYTE> ShaderCode;
	};
}
