#!/bin/sh

cd /ci_test
echo ""
echo "========================== FC V4H Test=============================="
echo ""
sleep 2
./frontcam_ref_app_v4h2&
PID=$!
sleep 10
kill -INT $PID 
wait $PID
echo "======================================================================"
echo "                      TEST COMPLETED "
echo "======================================================================"
