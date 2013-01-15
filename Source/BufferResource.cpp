#include "BufferResource.h"

BufferResource::BufferResource() : zFilePath(""), zBuffer(NULL)
{
	
}
BufferResource::BufferResource(string filePath, Buffer* buffer) : zFilePath(filePath), zBuffer(buffer)
{

}

BufferResource::~BufferResource()
{
	if(this->zBuffer) delete this->zBuffer; this->zBuffer = NULL;
}

