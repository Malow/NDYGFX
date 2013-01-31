#include "ObjectDataResource.h"

ObjectDataResource::ObjectDataResource() : zName(""), zObjData(NULL)
{

}
ObjectDataResource::ObjectDataResource(string ID, ObjData* objData) : zName(ID), zObjData(objData)
{

}

ObjectDataResource::~ObjectDataResource()
{
	if(this->zObjData) delete this->zObjData; this->zObjData = NULL;
}

