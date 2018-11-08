#include "Hook.h"

int CountFunction(void** vmt)
{
	MEMORY_BASIC_INFORMATION mem;

	int i = -1;

	do
	{
		i++;

		VirtualQuery(vmt[i], &mem, sizeof(MEMORY_BASIC_INFORMATION));
	} while (mem.Protect == PAGE_EXECUTE_READ || mem.Protect == PAGE_EXECUTE_READWRITE);

	return i;
}

CHookData::CHookData(void* vmt)
{
	m_vtable = (void***)vmt;

	m_oldtable = *m_vtable;

	m_vcount = CountFunction(m_oldtable);

	m_array = (void**)malloc((m_vcount + 3) * sizeof(void*));

	m_array[2] = m_oldtable[-1];

	for (int i = 0; i < m_vcount; i++)
		m_array[i + 3] = m_oldtable[i];

	*m_vtable = m_array + 3;
}

CHookData::~CHookData()
{
	if (m_vtable)
		Unhook();

	free(m_array);
}