#include "BufferResource.h"

BufferResource::BufferResource() : zName(""), zBuffer(NULL)
{
	
}
BufferResource::BufferResource(string ID, Buffer* buffer) : zName(ID), zBuffer(buffer)
{

}

BufferResource::~BufferResource()
{
	if(this->zBuffer) delete this->zBuffer; this->zBuffer = NULL;
}

