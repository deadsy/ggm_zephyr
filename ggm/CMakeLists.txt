cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

target_include_directories(app
	PRIVATE
		src/inc
		src/module
)

target_sources(app
	PRIVATE
		src/core/block.c
		src/core/event.c
		src/core/ftoa.c
		src/core/lut.c
		src/core/math.c
		src/core/midi.c
		src/core/module.c
		src/core/port.c
		src/core/synth.c
		src/module/template.c
		src/module/env/adsr.c
		src/module/midi/mon.c
		src/module/midi/poly.c
		src/module/osc/lfo.c
		src/module/osc/sine.c
		src/module/root/metro.c
		src/module/seq/seq.c
		src/module/voice/osc.c
		src/os/zephyr/zephyr.c
		src/os/zephyr/main.c
)
