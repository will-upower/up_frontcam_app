# up_frontcam_ref_app

Source code from Renesas's frontcam_ref_app. We will modify to use this with our own model and run on the v4h2 boards

# Running OCV imshow()

First things first is the config file for the frontcam_ref_app, in the frontcam_custom.config file. The parameter `VOUT_Enable` should be set to the default value, `0`, otherwise, the application will try to render the output via DRM directly to monitor. The OpenCV output window will only appear when the config is properly set up and the desktop display is on.

X11 is a pain, but Wayland is unworkable. Advise using gdm3, less of a hassle to set up as it works out of the box. For the purpose of this guide, I will detail setups for both display managers. I will not support wayland. Also works on via XRDP, but I recommend connecting the V4H2 Whitehawk directly to a monitor and running the front_cam_ref application via SSH or COM instead due to performance issues.

The command `xhost + local:root` must be run regardless of the setup. Without it, as root, either with the `sudo` command or in superuser mode, the .Xauthority file will be f*cked (I want to explain in more detail, but I'm not familiar with X enough to actually describe the specific issue).

However, even with running the xhost command, the monitor configuration needs to be correctly configured when running, otherwise the xorg server will not be able to determine the correct monitor to use when running the front_cam_ref application. Check the DISPLAY variable with `echo $DISPLAY`. If it is empty, then something is wrong. If you are using wayland, what is wrong with you? Anyways, run the command `export DISPLAY=:0.0`, which will tell the x server to use the first possible monitor connected to it. This step should only be necessary if you are trying to run the application remotely. Then run the `xhost + local:root` command again.
