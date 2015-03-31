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
#include <iWearSDK.h>

// Standard includes
// - none

namespace {
class TrackerInstance {
  public:
    TrackerInstance() { status = IWRLoadDll(); }

    /// @todo what happens if this is called when the DLL did not successfully
    /// open?
    ~TrackerInstance() { IWRFreeDll(); }

    void OpenTracker() { status = IWROpenTracker(); }

    void ZeroSet() { IWRZeroSet(); }

    void GetTracking(long &yaw, long &pitch, long &roll) {
        status = IWRGetTracking(&yaw, &pitch, &roll);
    }

    /// @todo DLL status accessor method (is DLL loaded?) and tracker status
    /// accessor method (have we opened a tracker?)

    long status;
};
} // namespace

#endif // INCLUDED_TrackerInstance_h_GUID_9253C2FC_831E_4D97_45C7_A681F310C5D8
