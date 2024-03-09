#pragma once

#include <Windows.h>

namespace MintChoco {
	template<typename T>
	class cSize {
	public:
		T Width;
		T Height;

		cSize() { SetZero(); }
		cSize(T _Width, T _Height) { Width = _Width; Height = _Height; }
		cSize(const SIZE& Size) { Width = static_cast<T>(Size.cx); Height = static_cast<T>(Size.cy); }
		~cSize() {}

		void SetZero() { Width = Height = 0; }

		template<typename U = T>
		void Scale(U Scale) { Width = static_cast<T>(static_cast<U>(Width) * Scale); Height = static_cast<T>(static_cast<U>(Height) * Scale); }

		template<typename U>
		cSize<U> operator()() { return cSize<U>(static_cast<U>(Width), static_cast<U>(Height)); }
	};
	using cSizei = cSize<int>;

	template<typename T>
	class cVolume {
	public:
		T X;
		T Y;
		T Z;

		cVolume() { SetZero(); }
		cVolume(T X, T Y, T Z) : X(X), Y(Y), Z(Z) {}
		cVolume(const cVolume& Other) { X = Other.X; Y = Other.Y; Z = Other.Z; }

		void SetZero() { X = Y = Z = static_cast<T>(0); }

		template<typename U = T>
		void Scale(U Scale) { X = static_cast<T>(static_cast<U>(X) * Scale); Y = static_cast<T>(static_cast<U>(Y) * Scale); Z = static_cast<T>(static_cast<U>(Z) * Scale); }

		template<typename U>
		cVolume<U> operator()() { return cVolume<U>(static_cast<U>(X), static_cast<U>(Y), static_cast<U>(Z)); }
	};
	using cVolumei = cVolume<int>;
	using cVolumeui = cVolume<UINT>;
}
