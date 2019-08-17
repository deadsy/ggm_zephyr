#!/bin/bash

PLATFORM=$(uname -m)

case $PLATFORM in
	x86_64)
		DEV="hw:CARD=PCH,DEV=0"
	;;
	armv7l)
		DEV="hw:CARD=sndrpihifiberry,DEV=0"
	;;
	*)
		echo "unknown platform"
		exit 1
	;;
esac

alsabat -P$DEV -f S16_LE -r 48000 -c 2
