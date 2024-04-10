#pragma once

#include <type_traits>

template<typename _TSeed>
_TSeed HashCombine(_TSeed Seed) {
	return Seed;
}

template<typename _TSeed, typename _TData, typename... _TRestSeed>
_TSeed HashCombine(_TSeed Seed, _TData Data, _TRestSeed... Args) {
	Seed = std::hash<_TData>{}(Data) +0x9e3779b9 + (Seed << 6) + (Seed >> 2);
	return HashCombine(Seed, Args...);
}