#!/bin/bash

PLATFORM=$(uname -m)
case $PLATFORM in
	x86_64)
		MIDI_CAPTURE_0=system:midi_capture_1
		PLAYBACK_0=system:playback_3
		PLAYBACK_1=system:playback_4
	;;
	armv7l)
		MIDI_CAPTURE_0=system:midi_capture_1
		PLAYBACK_0=system:playback_1
		PLAYBACK_1=system:playback_2
	;;
	*)
		echo "unknown platform"
		exit 1
	;;
esac

CONNECT=/usr/local/bin/jack_connect

$CONNECT $MIDI_CAPTURE_0 ggm:midi_in_0
$CONNECT $PLAYBACK_0 ggm:audio_out_0
$CONNECT $PLAYBACK_1 ggm:audio_out_1

exit $?
