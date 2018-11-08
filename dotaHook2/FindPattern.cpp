#include "FindPattern.h"

bool DataCompare(const BYTE* pbData, const BYTE* pbSig, const char* pszMask)
{
	for (; *pszMask; ++pszMask, ++pbData, ++pbSig)
	{
		if (*pszMask == 'x' && *pbData != *pbSig)
			return 0;
	}

	return (*pszMask == 0);
}

DWORD_PTR FindPattern(DWORD_PTR dwStart, DWORD_PTR dwSize, const char* pszSig, const char* pszMask)
{
	if (!dwSize)
		return 0;

	for (int i = 0; i < dwSize; i++)
	{
		if (DataCompare((PBYTE)(dwStart + i), (PBYTE)pszSig, pszMask))
			return (DWORD_PTR)(dwStart + i);
	}

	return 0;
}