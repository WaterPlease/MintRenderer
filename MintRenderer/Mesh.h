#pragma once

#include <string>
#include <vector>
#include <Windows.h>

namespace MintChoco {
#pragma pack(push, 4)
	struct sVertex {
		
	};
#pragma pack(pop)

	class cPrimitive {
	public:

	protected:
		std::vector<sVertex>	Vertices;
		std::vector<UINT>		Indecies;
	};
}

