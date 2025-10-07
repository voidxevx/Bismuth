#pragma once

#ifdef BISMUTH_BUILD_DLL
#define BISMUTH_API _declspec(dllexport)
#else
#define BISMUTH_API _declspec(dllimport)
#endif