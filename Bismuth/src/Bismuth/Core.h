#pragma once

#ifdef _WIN32
	#ifdef BISMUTH_BUILD_DLL
		#define BISMUTH_API _declspec(dllexport)
	#else
		#define BISMUTH_API _declspec(dllimport)
	#endif
#else
	#define BISMUTH_API
#endif

template<typename _T>
static void Delete_ptr(_T& ptr)
{
}
template<typename _T>
static void Delete_ptr(_T* ptr)
{
	delete ptr;
	ptr = nullptr;
}