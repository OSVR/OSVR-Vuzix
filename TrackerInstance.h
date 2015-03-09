/** @date 2015

@author
Sensics, Inc.
<http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#define IWEAR_ONETIME_DEFINITIONS
#include <iWearSDK.h>
#include <iostream>

class TrackerInstance
{
public:

	TrackerInstance(){
		status = IWRLoadDll();
	}

	~TrackerInstance(){
		IWRFreeDll();
	}

	long status;
};