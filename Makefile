all : flash

CH32V003FUN:=../ch32v003fun/ch32v003fun

TARGET_EXT:=cc
TARGET:=usart_tx

ADDITIONAL_C_FILES+=ringbuffer.cc

include $(CH32V003FUN)/ch32v003fun.mk

# Optimize for size.
# Because it optimizes the virtual functions out
# which are tested here.
CCFLAGS:= \
	-Os -flto -ffunction-sections \
	-static-libgcc \
	-march=rv32ec \
	-mabi=ilp32e \
	-I$(NEWLIB) \
	-I$(CH32V003FUN) \
	-nostdlib \
	-DCH32V003 \
	-I. -Wall

CCFLAGS+=-fno-rtti -DCPLUSPLUS

flash : cv_flash
clean : cv_clean

