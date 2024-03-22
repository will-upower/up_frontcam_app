#!/bin/bash

cd ..

timestamp=$(date +"%Y%m%d_%H%M%S")
build_log_file="up_frontcam_ref_app/${timestamp}_build_output.log"

./build_linux_dev_board.sh -d v4h2 -a up_frontcam_ref_app -b release 2>&1 | tee ${build_log_file}
