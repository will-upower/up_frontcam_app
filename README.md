# up_frontcam_ref_app

Modified Renesas frontcam_ref application. Modified to work on a desktop environment for the V4H2 SoC.

# Building

Depends on R-Car SDK ver 3.18 and the Renesas ai_lib package. They can be found [here](https://dynamap.sharepoint.com/:f:/s/HPVCUPUS/ErozWjPbst1Api6R42Sa0lIBKNxuYsulVBkbiXOMnN5XEw?e=FVQu7G).

Setup of the R-Car SDK should be straightforward, save for one potential hiccup. First, make sure to download the *.sh script, the poky toolchain *.tar.gz, the yocto linux *.tar.gz, the adas bootloader *.tar.gz, and the ai_lib addon *.zip. Put them in the home directory and run the build script. If their are any hash or checksum errors, manually install the tarballs to the default directory (~/Renesas/rcar-xos/v3.18.0/).

To install the ai_lib, extract the contents of it over the R-Car SDK. There are patch files, 2 for the cmake and 1 for the build script found in the samples folder. Patch them with this command: `patch -b < NAME_OF_PATCH.patch`

Finally, to build the application, clone this repository to the samples folder. Run the build script, and when asked for the name of the application, enter `up_frontcam_ref_app`. Target is V4H2 and built type should be Release.

# Running OCV imshow()

First things first is the config file for the frontcam_ref_app, in the frontcam_custom.config file. The parameter `VOUT_Enable` should be set to the default value, `0`, otherwise, the application will try to render the output via DRM directly to monitor. The OpenCV output window will only appear when the config is properly set up and the desktop display is on.

X11 is a pain, but Wayland is unworkable. Advise using gdm3, less of a hassle to set up as it works out of the box. For the purpose of this guide, I will detail setups for both display managers. I will not support wayland. Also works on via XRDP, but I recommend connecting the V4H2 Whitehawk directly to a monitor and running the front_cam_ref application via SSH or COM instead due to performance issues.

The command `xhost + local:root` must be run regardless of the setup. Without it, as root, either with the `sudo` command or in superuser mode, the .Xauthority file will be f*cked (I want to explain in more detail, but I'm not familiar with X enough to actually describe the specific issue).

However, even with running the xhost command, the monitor configuration needs to be correctly configured when running, otherwise the xorg server will not be able to determine the correct monitor to use when running the front_cam_ref application. Check the DISPLAY variable with `echo $DISPLAY`. If it is empty, then something is wrong. If you are using wayland, what is wrong with you? Anyways, run the command `export DISPLAY=:0.0`, which will tell the x server to use the first possible monitor connected to it. This step should only be necessary if you are trying to run the application remotely. Then run the `xhost + local:root` command again.
