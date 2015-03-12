/** @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Vuzix Corporation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// Needed in exactly one translation unit
#define IWEAR_ONETIME_DEFINITIONS
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
#include <memory>

// Anonymous namespace to avoid symbol collision
namespace {

typedef std::shared_ptr<TrackerInstance> TrackerPtr;

class VuzixDevice {
  public:
    VuzixDevice(OSVR_PluginRegContext ctx, TrackerPtr trackerInst) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        tracker = trackerInst;

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
        tracker->GetTracking(yaw, pitch, roll);

        if (tracker->status != IWR_OK) {
            std::cout << "PLUGIN: Vuzix tracker NOT connected, try again"
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        }

        OSVR_OrientationState trackerCoords =
            convYawPitchRollToQuat(yaw, pitch, roll);

        osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &trackerCoords, 0);
        return OSVR_RETURN_SUCCESS;
    }

    static OSVR_OrientationState convYawPitchRollToQuat(long yaw, long pitch,
                                                        long roll) {

        const double rawToRad = M_PI / 32768.0f;

        // convert raw values to radians
        double yawRad = yaw * rawToRad;
        double pitchRad = pitch * rawToRad;
        double rollRad = roll * rawToRad;

        double c1 = cos(yawRad / 2.0);
        double s1 = sin(yawRad / 2.0);

        double c2 = cos(pitchRad / 2.0);
        double s2 = sin(pitchRad / 2.0);

        double c3 = cos(rollRad / 2.0);
        double s3 = sin(rollRad / 2.0);

        double c1c2 = c1 * c2;
        double s1s2 = s1 * s2;

        double w = c1c2 * c3 - s1s2 * s3;
        double x = c1c2 * s3 + s1s2 * c3;
        double y = s1 * c2 * c3 + c1 * s2 * s3;
        double z = c1 * s2 * c3 - s1 * c2 * s3;

        OSVR_OrientationState trackerCoords = {w, z, y, x};

        return trackerCoords;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
    TrackerPtr tracker;
};

class HardwareDetection {
  public:
    HardwareDetection() : tracker(new TrackerInstance()) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

        if (tracker->status != IWR_OK) {
            std::cout << "PLUGIN: Could NOT load Vuzix tracker DLL"
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        }
        tracker->OpenTracker();

        if (tracker->status == IWR_OK) {
            std::cout << "PLUGIN: We have detected Vuzix device! " << std::endl;

            /// Create our device object
            osvr::pluginkit::registerObjectForDeletion(
                ctx, new VuzixDevice(ctx, tracker));
        } else {
            std::cout << "PLUGIN: We have NOT detected Vuzix tracker "
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        }
        return OSVR_RETURN_SUCCESS;
    }

    TrackerPtr tracker;
};
} // namespace

OSVR_PLUGIN(com_osvr_Vuzix) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}