#pragma once

#ifdef _WIN32
#ifdef BISMUTH_BUILD_DLL
#define BISMUTH_API __declspec(dllexport)
#else
#define BISMUTH_API __declspec(dllimport)
#endif
#else
#define BISMUTH_API 
#endif