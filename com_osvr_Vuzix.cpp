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

/// Needed to get M_PI from math.h on Windows.
#define _USE_MATH_DEFINES

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/Kalman/OrientationState.h>
#include <osvr/Kalman/AngularVelocityMeasurement.h>
#include <osvr/Kalman/OrientationConstantVelocity.h>
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include "TrackerInstance.h"

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

using ProcessModel = osvr::kalman::OrientationConstantVelocityProcessModel;
using FilterState = ProcessModel::State;
using AngularVelMeasurement =
    osvr::kalman::AngularVelocityMeasurement<FilterState>;
using osvr::kalman::types::Vector;
using osvr::util::time::duration;

namespace ei = osvr::util::eigen_interop;
class VuzixDevice {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    VuzixDevice(OSVR_PluginRegContext ctx, TrackerPtr trackerInst)
        : m_imuMeas(Vector<3>::Zero(), Vector<3>::Constant(1.0E-8))

    {
        osvrTimeValueGetNow(&m_last);
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        tracker = trackerInst;

        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the sync device token with the options
        m_dev.initSync(ctx, "Vuzix", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_Vuzix_json);
        std::cout << "Quat: " << m_state.getQuaternion().coeffs().transpose()
                  << std::endl;
        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode VuzixDevice::update() {

        iWearMagVector mag;
        iWearAccelVector accel;
        iWearGyroVector gyro;
        OSVR_ReturnCode ret = tracker->GetRawTracking(&mag, &accel, &gyro);
        Eigen::Vector3d angVel(gyro.x, gyro.y, gyro.z);
        OSVR_TimeValue now;
        osvrTimeValueGetNow(&now);
        preReport(now);
        m_imuMeas.setMeasurement(angVel);
        osvr::kalman::correct(m_state, m_model, m_imuMeas);
        // std::cout << "Quat: " << m_state.getQuaternion().coeffs().transpose()
        // << std::endl;
        // printf("[PLUGIN] x: %.2f; y: %.2f; z: %.2f; w: %.2f \n",
        // trackerCoords.data[1], trackerCoords.data[2], trackerCoords.data[3],
        // trackerCoords.data[0]);
        // osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &trackerCoords,
        // 0);
        return OSVR_RETURN_SUCCESS;
    }

    bool VuzixDevice::preReport(const OSVR_TimeValue &timestamp) {
        auto dt = duration(timestamp, m_last);
        if (dt > 0) {
            m_last = timestamp;
            // only predict if time has moved forward
            osvr::kalman::predict(m_state, m_model, dt);
        }
        return true;
    }

    static OSVR_OrientationState convEulerToQuat(long yaw, long pitch,
                                                 long roll) {

        const double rawToRad = M_PI / 32768.0f;

        // convert raw values to radians
        double yawRad = yaw * rawToRad;
        double pitchRad = pitch * rawToRad;
        double rollRad = roll * rawToRad;
        // invert roll to correct rolling direction
        rollRad *= -1.0;

        /* put angles into radians and divide by two, since all angles in
        * formula
        *  are (angle/2)
        */

        double half_yaw = yawRad / 2.0;
        double half_pitch = pitchRad / 2.0;
        double half_roll = rollRad / 2.0;

        double cosYaw = cos(half_yaw);
        double sinYaw = sin(half_yaw);

        double cosPitch = cos(half_pitch);
        double sinPitch = sin(half_pitch);

        double cosRoll = cos(half_roll);
        double sinRoll = sin(half_roll);

        double x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
        double y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
        double z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;

        double w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;

        OSVR_OrientationState trackerCoords = {w, y, z, x};

        return trackerCoords;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
    TrackerPtr tracker;
    FilterState m_state;
    ProcessModel m_model;
    AngularVelMeasurement m_imuMeas;
    OSVR_TimeValue m_last;
};

class HardwareDetection {
  public:
    HardwareDetection() : tracker(new TrackerInstance()) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

        OSVR_ReturnCode ret = tracker->OpenTracker();

        if (ret == OSVR_RETURN_SUCCESS) {
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