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

//#define IWEAR_ONETIME_DEFINITIONS
#define _USE_MATH_DEFINES

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <TrackerInstance.h>

// Generated JSON header file
#include "com_osvr_Vuzix_json.h"

// Library/third-party includes
#include <math.h>

// Standard includes
#include <iostream>

// Anonymous namespace to avoid symbol collision
namespace {

class VuzixDevice {
  public:
    VuzixDevice(OSVR_PluginRegContext ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the sync device token with the options
        m_dev.initSync(ctx, "Vuzix", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_Vuzix_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {

		long yaw, pitch, roll;
        long iwr_status = IWRGetTracking(&yaw, &pitch, &roll);
        
        if (iwr_status != IWR_OK){
            std::cout << "PLUGIN: Vuzix tracker NOT connected, try again" << std::endl;
			return OSVR_RETURN_FAILURE;
        }
        
        OSVR_OrientationState trackerCoords = convYawPitchRollToQuat(yaw, pitch, roll);
        
        osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &trackerCoords, 0);
        return OSVR_RETURN_SUCCESS;
    }

	static OSVR_OrientationState convYawPitchRollToQuat(long yaw, long pitch, long roll){

		const float rawToRad = M_PI / 32768.0f;

		//convert raw values to radians
		double yawRad = yaw * rawToRad;
		double pitchRad = pitch * rawToRad;
		double rollRad = roll * rawToRad;

		double c1 = cos(yawRad / 2.0);
		double s1 = sin(yawRad / 2.0);

		double c2 = cos(pitchRad / 2.0);
		double s2 = sin(pitchRad / 2.0);

		double c3 = cos(rollRad / 2.0);
		double s3 = sin(rollRad / 2.0);

		double c1c2 = c1*c2;
		double s1s2 = s1*s2;

		double w = c1c2*c3 - s1s2*s3;
		double x = c1c2*s3 + s1s2*c3;
		double y = s1*c2*c3 + c1*s2*s3;
		double z = c1*s2*c3 - s1*c2*s3;

		OSVR_OrientationState trackerCoords = { w, z, y, x };

		return trackerCoords;
	}

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
};

class HardwareDetection {
  public:
	  HardwareDetection() : trackerLoad(new TrackerInstance()) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

		//trackerLoad = new TrackerInstance();
		long iwr_status = trackerLoad->status;
        
        if (iwr_status != IWR_OK){
            std::cout << "PLUGIN: Could NOT load Vuzix tracker DLL" << std::endl;
            IWRFreeDll();
			return OSVR_RETURN_FAILURE;
        }
        
        iwr_status = IWROpenTracker();

        if (iwr_status == IWR_OK) {
            std::cout << "PLUGIN: We have detected Vuzix device! " << std::endl;

            /// Create our device object
            osvr::pluginkit::registerObjectForDeletion(
                ctx, new VuzixDevice(ctx));
        }
        else{
            std::cout << "PLUGIN: We have NOT detected Vuzix tracker " << std::endl;
			return OSVR_RETURN_FAILURE;
        }
        return OSVR_RETURN_SUCCESS;
    }
private:
	TrackerInstance* trackerLoad;
};
} // namespace

OSVR_PLUGIN(com_osvr_Vuzix) {
    osvr::pluginkit::PluginContext context(ctx);
    
    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}