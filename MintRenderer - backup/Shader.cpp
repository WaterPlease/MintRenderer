#include "pch.h"
#include "Shader.h"

#include <fstream>

using namespace MintChoco;

cShader::cShader() {

}

cShader::~cShader() {

}

bool cShader::Create(const std::wstring& Path) {
	std::ifstream FileStream;
	FileStream.open(Path, std::ios::binary);

	bool bResult = FileStream.is_open();

	FileStream.seekg(0, std::ios::end);
	size_t FileSize = FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	ShaderCode.resize(FileSize);
	FileStream.read(reinterpret_cast<char*>(ShaderCode.data()), FileSize);
	return true;
}

void cShader::Destroy() {
	ShaderCode.clear();
	ShaderCode.shrink_to_fit();
}
