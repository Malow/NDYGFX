#pragma once

#include "Process.h"
#include "Mesh.h"

class DeleteObjectEvent : public MaloW::ProcessEvent
{
private:
	Mesh* obj;

public:
	DeleteObjectEvent(Mesh* object)
	{
		this->obj = object;
	}
	virtual ~DeleteObjectEvent()
	{
		delete this->obj;
	}
};

class DxManagerHelperThread : public MaloW::Process
{
	private:

	public:
		DxManagerHelperThread();
		virtual ~DxManagerHelperThread();

		virtual void Life();
};