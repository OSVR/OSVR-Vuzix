#define IWEAR_ONETIME_DEFINITIONS
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

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "com_osvr_Vuzix_json.h"

// Library/third-party includes
#include <iWearSDK.h>

// Standard includes
#include <iostream>

#define PI					3.14159265358979f
#define RAWTORAD            PI/32768.0f

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
       
        iwr_status = IWRGetTracking(&yaw, &pitch, &roll);
        
        if (iwr_status != IWR_OK){
            std::cout << "PLUGIN: Vuzix tracker NOT connected, try again" << std::endl;
            IWROpenTracker();
        }
        
        const OSVR_OrientationState trackerCoords = convYawPitchRollToQuat(yaw, pitch, roll);
        
        osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &trackerCoords, 0);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
    double m_myVal;
    long yaw, pitch, roll;
    long iwr_status;

    const OSVR_OrientationState convYawPitchRollToQuat(long yaw, long pitch, long roll){

        double yawRad, pitchRad, rollRad;
        //vars for conversion to Quat
        double c1, s1, c2, s2, c3, s3, c1c2, s1s2, w, x, y, z;
        //convert raw values to radians
        yawRad = yaw * RAWTORAD;
        pitchRad = pitch * RAWTORAD;
        rollRad = roll * RAWTORAD;
        
        c1 = cos(yawRad / 2.0);
        s1 = sin(yawRad / 2.0);

        c2 = cos(pitchRad / 2.0);
        s2 = sin(pitchRad / 2.0);

        c3 = cos(rollRad / 2.0);
        s3 = sin(rollRad / 2.0);

        c1c2 = c1*c2;
        s1s2 = s1*s2;
        
        w = c1c2*c3  - s1s2*s3;
        x = c1c2*s3  + s1s2*c3;        
        y = s1*c2*c3 + c1*s2*s3;
        z = c1*s2*c3 - s1*c2*s3;

       // std::cout << "Quat values: w: " << w << ", x: " << x << ", y: " << y << ", z: " << z << std::endl;

        const OSVR_OrientationState trackerCoords = { w, z, y, x };
        
        return trackerCoords;
    }
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

        iwr_status = IWRLoadDll();
        
        if (iwr_status != IWR_OK){
            std::cout << "PLUGIN: Could NOT load Vuzix tracker DLL" << std::endl;
            IWRFreeDll();
            
        }
        
        iwr_status = IWROpenTracker();

        if (iwr_status == IWR_OK) {
            std::cout << "PLUGIN: We have detected Vuzix device! " << std::endl;
            m_found = true;

            /// Create our device object
            osvr::pluginkit::registerObjectForDeletion(
                ctx, new VuzixDevice(ctx));
        }
        else{
            std::cout << "PLUGIN: We have NOT detected Vuzix tracker " << std::endl;
        }
        return OSVR_RETURN_SUCCESS;
    }

  private:

    bool m_found;
    long iwr_status;
    
};
} // namespace

OSVR_PLUGIN(com_osvr_Vuzix) {
    osvr::pluginkit::PluginContext context(ctx);
    
    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}