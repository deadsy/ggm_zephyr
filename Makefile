TOP = $(PWD)

BUILD = build
APP= ggm

BOARD = mimxrt1020_evk
#BOARD = stm32f4_disco

ZEPHYR_BASE = $(TOP)/zephyr/zephyr
ZEPHYR_MODULES = $(TOP)/zephyr/hal_stm32
ZEPHYR_TOOLCHAIN_VARIANT = cross-compile
CROSS_COMPILE = /opt/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-

BINFILE = $(BUILD)/zephyr/zephyr.bin

JLINK = /opt/SEGGER/JLink/JLinkExe
JLINK_CMD = cmd.jlink

.PHONY: all
all: .stamp_cmake build

.PHONY: build
build: .stamp_cmake
	ninja -C $(BUILD)

.PHONY: config
config: .stamp_cmake
	ninja -C $(BUILD) menuconfig

.PHONY: flash_nxp
flash_nxp:
	echo "r\nloadfile $(BINFILE) 0x60000000\nq" > $(JLINK_CMD)
	$(JLINK) -if swd -speed auto -device MIMXRT1021xxx5A -CommanderScript $(JLINK_CMD)
	rm $(JLINK_CMD)

.PHONY: flash_st
flash_st:
	st-flash write $(BINFILE) 0x08000000
	st-flash reset

.PHONY: clean
clean:
	-rm -rf $(BUILD)
	-rm -rf .stamp*

.PHONY: init
init:
	git clone git@github.com:deadsy/zephyr.git
	git -C zephyr remote add upstream git@github.com:zephyrproject-rtos/zephyr.git

.stamp_cmake:
	-rm -rf $(BUILD)
	ZEPHYR_BASE=$(ZEPHYR_BASE) \
	ZEPHYR_TOOLCHAIN_VARIANT=$(ZEPHYR_TOOLCHAIN_VARIANT) \
	CROSS_COMPILE=$(CROSS_COMPILE) \
	cmake -GNinja -DZEPHYR_MODULES=$(ZEPHYR_MODULES) -DBOARD=$(BOARD) -S $(APP) -B $(BUILD)
	touch $@
