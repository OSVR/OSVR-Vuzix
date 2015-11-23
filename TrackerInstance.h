/** @file
    @brief Header

    @date 2015

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

#ifndef INCLUDED_TrackerInstance_h_GUID_9253C2FC_831E_4D97_45C7_A681F310C5D8
#define INCLUDED_TrackerInstance_h_GUID_9253C2FC_831E_4D97_45C7_A681F310C5D8

// Internal Includes
// - none

// Library/third-party includes
#include <iWear.h>

// Standard includes
#include <iostream>

namespace {
class TrackerInstance {
  public:
    TrackerInstance() { status = iWearConfigureDevice(&dev); }

    ~TrackerInstance() {}

    OSVR_ReturnCode OpenTracker() {
        status = iWearOpenTracker(&dev);

        if (status == IWR_OK) {
            return OSVR_RETURN_SUCCESS;
        } else {
            return OSVR_RETURN_FAILURE;
        }
    }

    OSVR_ReturnCode GetTracking(OSVR_OrientationState *orient) {
        iWearPositionVector pos;
        status = iWearGetTracking(&dev, &pos);
        if (status == IWR_OK) {
            orient->data[0] = pos.w;
            orient->data[1] = pos.x;
            orient->data[2] = pos.y;
            orient->data[3] = pos.z;
            return OSVR_RETURN_SUCCESS;
        }
        return OSVR_RETURN_FAILURE;
    }

    OSVR_ReturnCode GetRawTracking(iWearMagVector *mag, iWearAccelVector *accel,
                                   iWearGyroVector *gyro) {
        iWearPositionVector pos;
        status = iWearGetRawTracking(&dev, mag, accel, gyro);
        if (status == IWR_OK) {
            return OSVR_RETURN_SUCCESS;
        }
        return OSVR_RETURN_FAILURE;
    }

  private:
    iWearDevice dev;
    iWearReturnCode status;
};
} // namespace

#endif // INCLUDED_TrackerInstance_h_GUID_9253C2FC_831E_4D97_45C7_A681F310C5D8
