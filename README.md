# OSVR-Vuzix 
> Maintained at <https://github.com/OSVR/OSVR-Vuzix>
>
> For details, see <http://osvr.github.io>

This is a plugin for OSVR that provides access to the tracker data on Vuzix HMDs.

It is Windows-only, and requires the Vuzix SDK to build. See <http://www.vuzix.com/support/developer_program/> for details.

You will need to install the Vuzix [drivers and VR Manager](http://www.vuzix.com/support/downloads_drivers/) software on your computer before using this plugin.

Then, just put the `.dll` file from this plugin in the same directory of your OSVR server as the other plugins - usually something like `osvr-plugins-0` - and use the config file provided here as an example.


## Licenses
This plugin: Licensed under the Apache License, Version 2.0.

Note that since this builds on the Vuzix SDK, you are responsible for complying with the terms of that license.
