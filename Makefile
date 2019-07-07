TOP = $(PWD)

ZEPHYR_BASE = $(TOP)/zephyr/zephyr
ZEPHYR_TOOLCHAIN_VARIANT = cross-compile
CROSS_COMPILE = /opt/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-

BINFILE = $(BUILD)/zephyr/zephyr.bin

BOARD = mimxrt1020_evk
#BOARD = stm32f4_disco

BUILD = build
APP= ggm

.PHONY: all
all:
	-rm -rf $(BUILD)
	ZEPHYR_BASE=$(ZEPHYR_BASE) \
	ZEPHYR_TOOLCHAIN_VARIANT=$(ZEPHYR_TOOLCHAIN_VARIANT) \
	CROSS_COMPILE=$(CROSS_COMPILE) \
	cmake -GNinja -DBOARD=$(BOARD) -S $(APP) -B $(BUILD)
	ninja -C $(BUILD)

.PHONY: flash
flash:
	ZEPHYR_BASE=$(ZEPHYR_BASE) \
	ninja -C $(BUILD) flash

.PHONY: flash_st
flash_st:
	st-flash write $(BINFILE) 0x08000000
	st-flash reset

.PHONY: clean
clean:
	-rm -rf $(BUILD)

.PHONY: update
update:
	ZEPHYR_BASE=$(ZEPHYR_BASE) \
	west update

.PHONY: init
init:
	west init $(TOP)/zephyr
