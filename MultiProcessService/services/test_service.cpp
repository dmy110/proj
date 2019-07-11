#include <cstdio>
#include "service_base.h"

extern "C"
{
	void* test_service();	
}



void* test_service()
{
	return new ServiceBase();
}


