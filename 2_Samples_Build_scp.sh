cd ..
./build_linux_dev_board.sh -a up_frontcam_ref_app -d v4h2 -b release

cd up_frontcam_ref_app
scp build_linux_dev_board/frontcam_with_cdnn_ref_app_v4h2 ubuntu@192.168.100.87:/home/ubuntu/up_frontcam_ref_app/test_data
