#!/bin/sh

cd /ci_test
echo ""
echo "========================== FC V4H WITH CDNN Test=============================="
echo ""
sleep 2
./frontcam_with_cdnn_ref_app_v4h2&
PID=$!
sleep 20
kill $PID 
sleep 20
echo "======================================================================"
echo "                      TEST COMPLETED "
echo "======================================================================"
