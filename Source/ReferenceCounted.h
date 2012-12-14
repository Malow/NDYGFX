#pragma once

class ReferenceCounted
{
	unsigned int zRefCounter;
private:
	virtual ~ReferenceCounted() {}

public:
	ReferenceCounted() : zRefCounter(1) {}
	
	inline void IncreaseReferenceCount() { zRefCounter++; }
	inline void DecreaseReferenceCount() { zRefCounter--; if ( !zRefCounter ) delete this; }
};