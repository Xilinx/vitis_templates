##Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#SPDX-License-Identifier: MIT

ifndef XILINX_VITIS
$(error Environment variable XILINX_VITIS is required and should point to VITIS install area)
endif

SDA_FLOW = cpu_emu
HOST_SRCS = vadd.cpp 
HOST_EXE_DIR=.
HOST_EXE = vadd
HOST_CFLAGS = -g -Wall -DFPGA_DEVICE -DC_KERNEL 
HOST_LFLAGS = 

KERNEL_SRCS = krnl_vadd.cpp
KERNEL_NAME = krnl_vadd
KERNEL_DEFS = 
KERNEL_INCS = 
#set target device for XCLBIN
XDEVICE=xilinx_u200_gen3x16_xdma_2_202110_1
XDEVICE_REPO_PATH=
KEEP_TEMP=1
KERNEL_DEBUG=
XCLBIN_NAME=bin_vadd
HOST_CFLAGS+=-DTARGET_DEVICE=\"${XDEVICE}\"

DEV_ARCH := $(shell platforminfo -p $(PLATFORM) | grep 'FPGA Family' | sed 's/.*://' | sed '/ai_engine/d' | sed 's/^[[:space:]]*//')
LINK_OUTPUT := link.xclbin
ifeq ($(DEV_ARCH), versal)
LINK_OUTPUT := xsa
endif

ifeq (${SDA_FLOW},cpu_emu)
    TARGET = sw_emu
    CLCC_OPT += -t sw_emu
    XCLBIN = ${XCLBIN_NAME}_cpu_emu.xclbin
    LINK_XCLBIN = ${XCLBIN_NAME}_cpu_emu.${LINK_OUTPUT}
    XO = ${XCLBIN_NAME}_cpu_emu.xo
else ifeq (${SDA_FLOW},hw_emu)
    TARGET = hw_emu
    CLCC_OPT += -t hw_emu
    XCLBIN = ${XCLBIN_NAME}_hw_emu.xclbin
    LINK_XCLBIN = ${XCLBIN_NAME}_hw_emu.${LINK_OUTPUT}
    XO = ${XCLBIN_NAME}_hw_emu.xo
else ifeq (${SDA_FLOW},hw)
    TARGET = hw
    XCLBIN = ${XCLBIN_NAME}_hw.xclbin
    LINK_XCLBIN = ${XCLBIN_NAME}_hw.${LINK_OUTPUT}
    XO = ${XCLBIN_NAME}_hw.xo
    CLCC_OPT += -t hw
endif

HOST_ARGS = ${XCLBIN}

COMMON_DIR = common
include ${COMMON_DIR}/common.mk

