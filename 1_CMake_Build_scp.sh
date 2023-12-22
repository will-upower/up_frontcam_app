source /opt/poky/3.1.1/environment-setup-aarch64-poky-linux

rm -rf build
mkdir build
cd build
cmake -B . -S ..

