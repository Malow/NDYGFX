#pragma once

class ReferenceCounted
{
	unsigned int zRefCounter;
protected:
	virtual ~ReferenceCounted() {}

public:
	ReferenceCounted() : zRefCounter(1) {}
	
	inline void IncreaseReferenceCount() { zRefCounter++; }
	inline void DecreaseReferenceCount() { zRefCounter--; if ( !zRefCounter ) delete this; }
	inline unsigned int GetReferenceCount() { return this->zRefCounter; }
};