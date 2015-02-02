#!/system/bin/sh

TS_DEV=/dev/input/event3

do_touch() {
	x=$1
	y=$2

	/system/bin/sendevent $TS_DEV 3 57 29		#EV_ABS ABS_MT_TRACKING_ID 29
	/system/bin/sendevent $TS_DEV 3 53 $x		#EV_ABS ABS_MT_POSITION_X 
	/system/bin/sendevent $TS_DEV 3 54 $y		#EV_ABS ABS_MT_POSITION_Y 
	/system/bin/sendevent $TS_DEV 3 48 29		#EV_ABS ABS_MT_TOUCH_MAJOR 29
	/system/bin/sendevent $TS_DEV 3 58 2		#EV_ABS ABS_MT_PRESSURE 2
	/system/bin/sendevent $TS_DEV 0 0 0		#EV_SYN SYN_REPORT 0
	/system/bin/sendevent $TS_DEV 3 57 4294967295	#EV_ABS ABS_MT_TRACKING_ID
	/system/bin/sendevent $TS_DEV 0 0 0		#EV_SYN SYN_REPORT 0
}

while :; do
	dmesg -c >/dev/null 2>&1
	# start camera
	do_touch 916 520

	/system/bin/sleep 5

	# effect HuaiJiu
	do_touch 15 57
	do_touch 174 263

	/system/bin/sleep 2

	# recorder
	do_touch 958 22

	# wait until the camera startup
	/system/bin/sleep 3

	if dmesg | busybox grep "200ms timeout" >/dev/null 2>&1; then
		echo "=======test fail 11"
		break
	fi

	# effect ZhengChang
	do_touch 15 57
	do_touch 195 105

	/system/bin/sleep 2

	# capture
	do_touch 950 75

	/system/bin/sleep 5

	# take a picture
	do_touch 958 378

	/system/bin/sleep 5

	dmesg -c >/dev/null 2>&1

	/system/bin/sleep 3

	if dmesg | busybox grep "200ms timeout" >/dev/null 2>&1; then
		echo "=======test fail 22"
		break
	fi

	# quit the camera
	input keyevent 4	#KEYCODE_BACK

	/system/bin/sleep 2
done

