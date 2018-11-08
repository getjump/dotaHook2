#ifndef HOOK_H
#define HOOK_H

#include <windows.h>

inline void**& GetVTable(void* instance)
{
	return *reinterpret_cast<void***>((size_t)instance);
}

inline const void** GetVTable(const void* instance)
{
	return *reinterpret_cast<const void***>((size_t)instance);
}

template<typename T>
inline T GetMethod(const void* instance, size_t index)
{
	return reinterpret_cast<T> (GetVTable(instance)[index]);
}

inline const void** GetVTableDirect(const void* instance)
{
	return reinterpret_cast<const void**>((size_t)instance);
}

template<typename T>
inline T GetMethodDirect(const void* instance, size_t index)
{
	return reinterpret_cast<T> (GetVTableDirect(instance)[index]);
}

int CountFunction(void** pInstance);

class CHookData
{
	CHookData(const CHookData&);

	CHookData& operator = (const CHookData&);

public:

	CHookData(void* vmt);

	~CHookData();

	inline void HookMethod(void* pHookedFunction, size_t nIndex)
	{
		m_array[nIndex + 3] = pHookedFunction;
	}

	inline void Unhook()
	{
		*m_vtable = m_oldtable;
	}

	inline void Rehook()
	{
		*m_vtable = m_array + 3;
	}

	inline bool Hooked() const
	{
		return *m_vtable != m_oldtable;
	}

	inline void EraseHooks()
	{
		for (int i = 0; i < m_vcount; i++)
			m_array[i + 3] = m_vtable[i];
	}

	inline int FunctionCount() const
	{
		return m_vcount;
	}

	inline void Poof()
	{
		m_vtable = 0;
	}

	template<typename T>
	inline T GetMethod(size_t nIndex) const
	{
		return (T)m_oldtable[nIndex];
	}

private:
	void***	m_vtable;
	void**	m_oldtable;
	void**	m_array;
	size_t	m_vcount;
};

class CVMTHookManager
{
public:
	CVMTHookManager(void)
	{
		memset(this, 0, sizeof(CVMTHookManager));
	}

	CVMTHookManager(PDWORD* ppdwClassBase)
	{
		bInitialize(ppdwClassBase);
	}

	CVMTHookManager(PDWORD** ppdwClassBase)
	{
		bInitialize(ppdwClassBase);
	}

	~CVMTHookManager(void)
	{
		UnHook();
	}
	bool bInitialize(PDWORD* ppdwClassBase)
	{
		m_ppdwClassBase = ppdwClassBase;
		m_pdwOldVMT = *ppdwClassBase;
		m_dwVMTSize = CountFunction((void**)ppdwClassBase);
		m_pdwNewVMT = new DWORD[m_dwVMTSize];
		memcpy(m_pdwNewVMT, m_pdwOldVMT, sizeof(DWORD) * m_dwVMTSize);
		*ppdwClassBase = m_pdwNewVMT;
		return true;
	}
	bool bInitialize(PDWORD** pppdwClassBase) // fix for pp
	{
		return bInitialize(*pppdwClassBase);
	}

	void UnHook(void)
	{
		if (m_ppdwClassBase)
		{
			*m_ppdwClassBase = m_pdwOldVMT;
		}
	}

	void ReHook(void)
	{
		if (m_ppdwClassBase)
		{
			*m_ppdwClassBase = m_pdwNewVMT;
		}
	}

	int iGetFuncCount(void)
	{
		return (int)m_dwVMTSize;
	}

	DWORD dwGetMethodAddress(int Index)
	{
		if (Index >= 0 && Index <= (int)m_dwVMTSize && m_pdwOldVMT != NULL)
		{
			return m_pdwOldVMT[Index];
		}
		return NULL;
	}

	PDWORD pdwGetOldVMT(void)
	{
		return m_pdwOldVMT;
	}

	DWORD dwHookMethod(DWORD dwNewFunc, unsigned int iIndex)
	{
		if (m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0)
		{
			m_pdwNewVMT[iIndex] = dwNewFunc;
			return m_pdwOldVMT[iIndex];
		}

		return NULL;
	}

private:
	DWORD dwGetVMTCount(PDWORD pdwVMT)
	{
		DWORD dwIndex = 0;

		for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
		{
			if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
			{
				break;
			}
		}
		return dwIndex;
	}
	PDWORD*    m_ppdwClassBase;
	PDWORD    m_pdwNewVMT, m_pdwOldVMT;
	DWORD    m_dwVMTSize;
};

#endif // HOOK_H