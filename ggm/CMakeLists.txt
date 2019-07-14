cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

target_include_directories(app
	PRIVATE
		src/inc
)

target_sources(app
	PRIVATE
		src/core/main.c
		src/core/ftoa.c
		src/core/math.c
		src/core/lut.c
		src/core/module.c
		src/module/env/adsr.c
		src/module/osc/sine.c
)
