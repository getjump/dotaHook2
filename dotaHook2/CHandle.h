#ifndef EHANDLE_H
#define EHANDLE_H
#ifdef _WIN32
#pragma once
#endif

// How many bits to use to encode an edict.
#define	MAX_EDICT_BITS				11			// # of bits needed to represent max edicts

#define NUM_ENT_ENTRY_BITS		(MAX_EDICT_BITS + 1)
#define NUM_ENT_ENTRIES			(1 << NUM_ENT_ENTRY_BITS)
#define ENT_ENTRY_MASK			(NUM_ENT_ENTRIES - 1)
#define INVALID_EHANDLE_INDEX	0xFFFFFFFF

template <typename T>
class CHandle_s
{
public:
	unsigned long m_Index;

	inline int GetEntryIndex() const
	{
		return m_Index & ENT_ENTRY_MASK;
	}

	inline int GetSerialNumber() const
	{
		return m_Index >> NUM_ENT_ENTRY_BITS;
	}

};

#endif