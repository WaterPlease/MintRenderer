#pragma once

#include <cassert>
#include <Windows.h>
#include <algorithm>

namespace MintChoco {
	template<typename T>
	class cSizeT {
	public:
		T Width;
		T Height;

		cSizeT() { SetZero(); }
		cSizeT(T _Width, T _Height) { Width = _Width; Height = _Height; }
		cSizeT(const SIZE& Size) { Width = static_cast<T>(Size.cx); Height = static_cast<T>(Size.cy); }
		~cSizeT() {}

		void SetZero() { Width = Height = 0; }

		template<typename U = T>
		void Scale(U Scale) { Width = static_cast<T>(static_cast<U>(Width) * Scale); Height = static_cast<T>(static_cast<U>(Height) * Scale); }

		template<typename U>
		cSizeT<U> operator()() { return cSizeT<U>(static_cast<U>(Width), static_cast<U>(Height)); }
	};
	using cSizei = cSizeT<int>;

	template<typename T>
	class cVolumeT {
	public:
		T X;
		T Y;
		T Z;

		cVolumeT() { SetZero(); }
		cVolumeT(T X, T Y, T Z) : X(X), Y(Y), Z(Z) {}
		cVolumeT(const cVolumeT& Other) { X = Other.X; Y = Other.Y; Z = Other.Z; }

		void SetZero() { X = Y = Z = static_cast<T>(0); }

		template<typename U = T>
		void Scale(U Scale) { X = static_cast<T>(static_cast<U>(X) * Scale); Y = static_cast<T>(static_cast<U>(Y) * Scale); Z = static_cast<T>(static_cast<U>(Z) * Scale); }

		template<typename U>
		cVolumeT<U> operator()() { return cVolumeT<U>(static_cast<U>(X), static_cast<U>(Y), static_cast<U>(Z)); }
	};
	using cVolumei = cVolumeT<int>;
	using cVolumeui = cVolumeT<UINT>;

	template<typename T>
	class cRangeT {
	public:
		T Start;
		T End;

		cRangeT() { Start = End = static_cast<T>(0); }
		cRangeT(T _Start, T _End) { Start = _Start; End = _End; }

		void Normalize() { if (Start > End) std::swap(Start, End); }
		bool IsIn(T Value) const { return Start <= Value && End < Value; }
	};

	template<typename T>
	class cRingRangeT {
	public:
		T Start;
		T End;

		T Circumference;

		cRingRangeT(T _Circumference) { Circumference = _Circumference; Start = End = static_cast<T>(0); }
		cRingRangeT(T _Circumference, T _Start) { Circumference = _Circumference; Start = _Start; End = _Start; }

		T Distance(T From, T To) const {
			return ((To + Circumference) - From) % Circumference;
		}

		bool IsIn(T Value) const {
			Value = Value % Circumference;
			T FromStartToValue = Distance(Start, Value);
			T FromEndToValue = Distance(Start, Value);

			return FromStartToValue < FromEndToValue;
		}

		bool IsIntersect(const cRingRangeT<T>& Other) {
			assert(Circumference == Other.Circumference);

			if (IsIn(Other.Start) || IsIn((Other.End + Circumference - 1) % Circumference))
				return true;

			if (Other.IsIn(Start) || Other.IsIn((End + Circumference - 1) % Circumference))
				return true;

			return false;
		}

		bool IsEmpty() const { return End == Start; }

	protected:
		cRingRangeT() = delete;
	};
	using cRingRange = cRingRangeT<size_t>;
}
