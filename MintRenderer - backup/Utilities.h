#pragma once

#include <chrono>

#define __DECLARE_ENUM_NEXT_POSTFIX_(EnumClass) \
	EnumClass operator++(EnumClass& EnumValue, int);
#define __DEFINE_ENUM_NEXT_POSTFIX_(NameSpace, EnumClass) \
	EnumClass NameSpace::operator++(EnumClass& EnumValue, int) {\
		EnumClass Result = EnumValue; \
		++Result; \
		return Result; \
	}

#define DECLARE_ENUM_NEXT_ADD(EnumClass, EndValue) \
	EnumClass operator++(EnumClass& EnumValue);\
	__DECLARE_ENUM_NEXT_POSTFIX_(EnumClass)
#define DEFINE_ENUM_NEXT_ADD(NameSpace, EnumClass, EndValue) \
	EnumClass NameSpace::operator++(EnumClass& EnumValue) {\
		if (static_cast<int>(EnumValue) >= static_cast<int>(EndValue)) {\
			EnumValue = EndValue; \
		}\
		else {\
			EnumValue = static_cast<EnumClass>((static_cast<int>(EnumValue) + 1));\
		}\
		return EnumValue;\
	}\
	__DEFINE_ENUM_NEXT_POSTFIX_(NameSpace, EnumClass)

#define DECLARE_ENUM_NEXT_SHIFT(EnumClass, EndValue) \
	EnumClass operator++(EnumClass& EnumValue);\
	__DECLARE_ENUM_NEXT_POSTFIX_(EnumClass)
#define DEFINE_ENUM_NEXT_SHIFT(NameSpace, EnumClass, EndValue) \
	EnumClass NameSpace::operator++(EnumClass& EnumValue) {\
		if (static_cast<int>(EnumValue) >= static_cast<int>(EndValue)) {\
			EnumValue = EndValue; \
		}\
		else {\
			EnumValue = static_cast<EnumClass>((static_cast<int>(EnumValue) << 1));\
		}\
		return EnumValue;\
	}\
	__DEFINE_ENUM_NEXT_POSTFIX_(NameSpace, EnumClass)

#define RETURN_IF_CREATED() \
	if (IsCreated()) { \
		return;\
	}
#define RETURN_IF_NOT_CREATED() \
	if (!IsCreated()) { \
		return;\
	}

#define RETURN_FALSE_IF_CREATED() \
	if (IsCreated()) { \
		return false;\
	}
#define RETURN_FALSE_IF_NOT_CREATED() \
	if (!IsCreated()) { \
		return false;\
	}

#define RETURN_NULLPTR_IF_CREATED() \
	if (IsCreated()) { \
		return nullptr;\
	}
#define RETURN_NULLPTR_IF_NOT_CREATED() \
	if (!IsCreated()) { \
		return nullptr;\
	}

#define RETURN_NULLOPT_IF_CREATED() \
	if (IsCreated()) { \
		return std::nullopt;\
	}
#define RETURN_NULLOPT_IF_NOT_CREATED() \
	if (!IsCreated()) { \
		return std::nullopt;\
	}

#define ASSERT_RETURN_IF_CREATED() \
	if (IsCreated()) { \
		assert(false);\
		return;\
	}
#define ASSERT_RETURN_IF_NOT_CREATED() \
	if (!IsCreated()) { \
		assert(false);\
		return;\
	}

#define ASSERT_RETURN_FALSE_IF_CREATED() \
	if (IsCreated()) { \
		assert(false);\
		return false;\
	}
#define ASSERT_RETURN_FALSE_IF_NOT_CREATED() \
	if (!IsCreated()) { \
		assert(false);\
		return false;\
	}

#define ASSERT_RETURN_NULLPTR_IF_CREATED() \
	if (IsCreated()) { \
		assert(false);\
		return nullptr;\
	}
#define ASSERT_RETURN_NULLPTR_IF_NOT_CREATED() \
	if (!IsCreated()) { \
		assert(false);\
		return nullptr;\
	}

#define ASSERT_RETURN_NULLOPT_IF_CREATED() \
	if (IsCreated()) { \
		assert(false);\
		return std::nullopt;\
	}
#define ASSERT_RETURN_NULLOPT_IF_NOT_CREATED() \
	if (!IsCreated()) { \
		assert(false);\
		return std::nullopt;\
	}


#define DELETE_CLASS_COPY(ClassName) \
	public:\
	ClassName(const ClassName&) = delete; \
	ClassName& operator=(const ClassName&) = delete;

// Debug 
#ifdef _DEBUG
	#define IF_DEBUG(TrueExpr, FalseExpr)	(TrueExpr)
	#define IFN_DEBUG(TrueExpr, FalseExpr)	(FalseExpr)
#else
	#define IF_DEBUG(TrueExpr, FalseExpr)	(FalseExpr)
	#define IFN_DEBUG(TrueExpr, FalseExpr)	(TrueExpr)
#endif

class cFrameLimiter {
public:
	using cTimePoint = std::chrono::time_point<std::chrono::steady_clock>;
	cFrameLimiter();

	void SetMaxFPS(size_t FPS);

	void Wait(bool bUpdateStartTime = true);
protected:
	size_t MaxFPS;
	size_t MinFrameTime;

	cTimePoint StartTime;
	cTimePoint FinishTime;

	static cTimePoint GetTime();
	static size_t GetInterval(const cTimePoint& StartTime, const cTimePoint& FinishTime);
};