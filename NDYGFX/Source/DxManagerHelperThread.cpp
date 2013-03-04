#include "DxManagerHelperThread.h"


DxManagerHelperThread::DxManagerHelperThread()
{

}

DxManagerHelperThread::~DxManagerHelperThread()
{

}

void DxManagerHelperThread::Life()
{
	while(this->stayAlive)
	{
		MaloW::ProcessEvent* ev = this->WaitEvent();
		if(DeleteObjectEvent* delEv = dynamic_cast<DeleteObjectEvent*>(ev))
		{
			// Do nothing, let the event delete whatever is in it.
		}

		delete ev;
	}
}
