/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#pragma once

#include <StdInc.h>
#include <algorithm>

 //
 // Returns the Citizen root directory.
 //

fwPlatformString GetAbsoluteCitPath();

//
// Returns the game root directory.
//

fwPlatformString GetAbsoluteGamePath();

//
// Returns a said path relative to the Citizen base installation directory (containing the launcher executable).
//

inline fwPlatformString MakeRelativeCitPath(fwPlatformString targetPath)
{
	return GetAbsoluteCitPath() + targetPath;
}

//
// Returns a said path relative to the game installation directory (containing the target executable).
//

inline fwPlatformString MakeRelativeGamePath(fwPlatformString targetPath)
{
	return GetAbsoluteGamePath() + targetPath;
}

//
// Returns whether or not we are in a test executable.
//

bool IsRunningTests();

//
// Base initialization function (for running on library init and so on)
//

class STATIC InitFunctionBase
{
protected:
	InitFunctionBase* m_next;

	int m_order;

public:
	InitFunctionBase(int order = 0);

	virtual void Run() = 0;

	void Register();

	static void RunAll();
};

//
// Initialization function that will be called around initialization of the primary component.
//

class InitFunction : public InitFunctionBase
{
private:
	void(*m_function)();

public:
	InitFunction(void(*function)(), int order = 0)
		: InitFunctionBase(order)
	{
		m_function = function;

		Register();
	}

	virtual void Run()
	{
		m_function();
	}
};

//
// 'compile-time' hashing
//
/*
template <unsigned int N, unsigned int I>
struct FnvHash
{
	FORCEINLINE static unsigned int Hash(const char(&str)[N])
	{
		return (FnvHash<N, I - 1>::Hash(str) ^ str[I - 1]) * 16777619u;
	}
};

template <unsigned int N>
struct FnvHash<N, 1>
{
	FORCEINLINE static unsigned int Hash(const char(&str)[N])
	{
		return (2166136261u ^ str[0]) * 16777619u;
	}
};

class StringHash
{
private:
	unsigned int m_hash;

public:
	template <unsigned int N>
	FORCEINLINE StringHash(const char(&str)[N])
		: m_hash(FnvHash<N, N>::Hash(str))
	{
	}

	FORCEINLINE operator unsigned int() const
	{
		return m_hash;
	}
};

#ifndef _M_AMD64
#define CALL_NO_ARGUMENTS(addr) ((void(*)())addr)()
#define EAXJMP(a) { _asm mov eax, a _asm jmp eax }
#define WRAPPER __declspec(naked)
#else
#define EAXJMP(a)
#define WRAPPER
#endif

//
// export class specifiers
//

#ifdef COMPILING_GAME
#define GAME_EXPORT DLL_EXPORT
#else
#define GAME_EXPORT DLL_IMPORT
#endif

#ifdef COMPILING_HOOKS
#define HOOKS_EXPORT DLL_EXPORT
#else
#define HOOKS_EXPORT DLL_IMPORT
#endif

#ifdef COMPILING_CORE
#define CORE_EXPORT DLL_EXPORT
#else
#define CORE_EXPORT DLL_IMPORT
#endif

#ifdef COMPILING_GAMESPEC
#define GAMESPEC_EXPORT DLL_EXPORT
#define GAMESPEC_EXPORT_VMT DLL_EXPORT
#else
#define GAMESPEC_EXPORT DLL_IMPORT
#define GAMESPEC_EXPORT_VMT
#endif

//
// formatting/logging functions
//

#include <fmt/format.h>
#include <fmt/printf.h>

template<typename TEnum, typename = std::enable_if_t<std::is_enum<TEnum>::value>>
std::ostream& operator<<(std::ostream& os, const TEnum& value)
{
	os << static_cast<typename std::underlying_type_t<TEnum>>(value);
	return os;
}

const char* va(const char* string, const fmt::ArgList& formatList);
FMT_VARIADIC(const char*, va, const char*);

void TraceReal(const char* channel, const char* func, const char* file, int line, const char* string, const fmt::ArgList& formatList);
FMT_VARIADIC(void, TraceReal, const char*, const char*, const char*, int, const char*);

#if defined(COMPILING_ADHESIVE) || defined(COMPILING_SVADHESIVE)
#define _CFX_TRACE_FILE "adhesive"
#define _CFX_TRACE_FUNC "adhesive"
#else
#define _CFX_TRACE_FILE __FILE__
#define _CFX_TRACE_FUNC __func__
#endif

#ifndef _CFX_COMPONENT_NAME
#ifdef COMPILING_SHARED
#define _CFX_COMPONENT_NAME Shared
#elif defined(COMPILING_SHARED_LIBC)
#define _CFX_COMPONENT_NAME SharedLibc
#elif defined(COMPILING_LAUNCHER)
#define _CFX_COMPONENT_NAME Launcher
#elif defined(COMPILING_CORE)
#define _CFX_COMPONENT_NAME CitiCore
#else
#define _CFX_COMPONENT_NAME Any
#endif
#endif

#define _CFX_NAME_STRING_(x) #x
#define _CFX_NAME_STRING(x) _CFX_NAME_STRING_(x)

#define trace(f, ...) TraceReal(_CFX_NAME_STRING(_CFX_COMPONENT_NAME), _CFX_TRACE_FUNC, _CFX_TRACE_FILE, __LINE__, f, ##__VA_ARGS__)

const wchar_t* va(const wchar_t* string, const fmt::ArgList& formatList);
FMT_VARIADIC_W(const wchar_t*, va, const wchar_t*);

// hash string, don't lowercase
inline constexpr uint32_t HashRageString(const char* string)
{
	uint32_t hash = 0;

	for (; *string; ++string)
	{
		hash += *string;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

inline constexpr char ToLower(const char c)
{
	return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
}

// hash string, lowercase
inline constexpr uint32_t HashString(const char* string)
{
	uint32_t hash = 0;

	for (; *string; ++string)
	{
		hash += ToLower(*string);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

inline void LowerString(fwString& string)
{
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);
}

fwString url_encode(const fwString& value);
bool UrlDecode(const std::string& in, std::string& out);
void CreateDirectoryAnyDepth(const char* path);

void SetThreadName(int threadId, const char* threadName);

std::wstring ToWide(const std::string& narrow);
std::string ToNarrow(const std::wstring& wide);

#ifdef COMPILING_CORE
extern "C" bool DLL_EXPORT CoreIsDebuggerPresent();
extern "C" void DLL_EXPORT CoreSetDebuggerPresent();
extern "C" void DLL_EXPORT CoreTrace(const char* channel, const char* func, const char* file, int line, const char* string);
#elif _WIN32
inline bool CoreIsDebuggerPresent()
{
	static bool(*func)();

	if (!func)
	{
		func = (bool(*)())GetProcAddress(GetModuleHandle("CoreRT.dll"), "CoreIsDebuggerPresent");
	}

	return (!func) ? false : func();
}

inline void CoreSetDebuggerPresent()
{
	static void(*func)();

	if (!func)
	{
		func = (void(*)())GetProcAddress(GetModuleHandle("CoreRT.dll"), "CoreSetDebuggerPresent");
	}

	(func) ? func() : (void)0;
}

inline void CoreTrace(const char* channel, const char* funcName, const char* file, int line, const char* string)
{
	using TCoreTraceFunc = decltype(&CoreTrace);

	static TCoreTraceFunc func;

	if (!func)
	{
		func = (TCoreTraceFunc)GetProcAddress(GetModuleHandle(L"CoreRT.dll"), "CoreTrace");
	}

	(func) ? func(channel, funcName, file, line, string) : (void)0;
}
#else
inline bool CoreIsDebuggerPresent()
{
	return false;
}

inline void CoreSetDebuggerPresent()
{

}

extern "C" void CoreTrace(const char* channel, const char* funcName, const char* file, int line, const char* string);
#endif


// min/max
template<typename T, typename = void>
struct MinMax
{
	inline static T min(T a, T b)
	{
		return (a < b) ? a : b;
	}

	inline static T max(T a, T b)
	{
		return (a > b) ? a : b;
	}
};

template<typename TValue>
struct MinMax<TValue, std::enable_if_t<std::is_integral<TValue>::value>>
{
	using TSigned = std::make_signed_t<TValue>;

	inline static TValue min(TValue aa, TValue bb)
	{
		TSigned a = (TSigned)aa;
		TSigned b = (TSigned)bb;

		return b + ((a - b) & (a - b) >> (sizeof(TSigned) * std::numeric_limits<TSigned>::max() - 1));
	}

	inline static TValue max(TValue aa, TValue bb)
	{
		TSigned a = (TSigned)aa;
		TSigned b = (TSigned)bb;

		return a - ((a - b) & (a - b) >> (sizeof(TSigned) * std::numeric_limits<TSigned>::max() - 1));
	}
};

#ifdef _MSC_VER
template<>
struct MinMax<float>
{
	inline static float min(float a, float b)
	{
		_mm_store_ss(&a, _mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)));
		return a;
	}

	inline static float max(float a, float b)
	{
		_mm_store_ss(&a, _mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)));
		return a;
	}
};

template<>
struct MinMax<double>
{
	inline static double min(double a, double b)
	{
		_mm_store_sd(&a, _mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)));
		return a;
	}

	inline static double max(double a, double b)
	{
		_mm_store_sd(&a, _mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)));
		return a;
	}
};
#endif

template<typename T>
inline T fwMin(T a, T b)
{
	return MinMax<T>::min(a, b);
}

template<typename T>
inline T fwMax(T a, T b)
{
	return MinMax<T>::max(a, b);
}
*/