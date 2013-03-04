#pragma once

#include "Process.h"

class DeleteObjectEvent : public MaloW::ProcessEvent
{
private:
	void* obj;

public:
	DeleteObjectEvent(void* object)
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