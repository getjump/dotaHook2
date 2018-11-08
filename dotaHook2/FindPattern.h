#ifndef FINDPATTERN_H
#define FINDPATTERN_H

#include <Windows.h>

DWORD_PTR FindPattern(DWORD_PTR dwStart, DWORD_PTR dwSize, const char* pszSig, const char* pszMask);
#endif