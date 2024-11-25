##Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
#SPDX-License-Identifier: MIT

SHELL = /bin/bash
VPATH = ./

#supported flow: cpu_emu, hw_emu, hw
CC = g++
CLCC = v++
PACKAGE_OUT = ./package.$(TARGET)

ifeq ($(XDEVICE_REPO_PATH),)
#no device repo path set. do nothing
    DEVICE_REPO_OPT = 
else
    DEVICE_REPO_OPT = --xp prop:solution.platform_repo_paths=${XDEVICE_REPO_PATH} 
endif

HOST_CFLAGS += -I${XILINX_XRT}/include/
HOST_LFLAGS += -L${XILINX_XRT}/lib/ -lxilinxopencl -lrt -pthread
CLCC_OPT += $(CLCC_OPT_LEVEL) ${DEVICE_REPO_OPT} --platform ${XDEVICE} ${KERNEL_DEFS} ${KERNEL_INCS}

ifeq (${KEEP_TEMP},1)
    CLCC_OPT += -s
endif

ifeq (${KERNEL_DEBUG},1)
    CLCC_OPT += -g
endif

ifeq (${SDA_FLOW},cpu_emu)
	EMU_MODE=sw_emu
else ifeq (${SDA_FLOW},hw_emu)
	EMU_MODE=hw_emu
endif

CLCC_OPT += --kernel ${KERNEL_NAME}
OBJECTS := $(HOST_SRCS:.cpp=.o)

.PHONY: all

all: run

host: ${HOST_EXE_DIR}/${HOST_EXE}

xbin_cpu_em:
	make SDA_FLOW=cpu_emu xbin -f vitis.mk

xbin_hw_em:
	make SDA_FLOW=hw_emu xbin -f vitis.mk

xbin_hw :
	make SDA_FLOW=hw xbin -f vitis.mk

xbin: ${XCLBIN}

run_cpu_em: 
	make SDA_FLOW=cpu_emu run_em -f vitis.mk

run_hw_em: 
	make SDA_FLOW=hw_emu run_em -f vitis.mk

run_hw : 
	make SDA_FLOW=hw run_hw_int -f vitis.mk

run_em: xconfig host xbin
	XCL_EMULATION_MODE=${EMU_MODE} ${HOST_EXE_DIR}/${HOST_EXE} ${HOST_ARGS}

run_hw_int : host xbin_hw
	${HOST_EXE_DIR}/${HOST_EXE} ${HOST_ARGS}

estimate : 
	${CLCC} -c -t hw_emu --platform ${XDEVICE} --report estimate ${KERNEL_SRCS}

xconfig : emconfig.json

emconfig.json :
	emconfigutil --platform ${XDEVICE} ${DEVICE_REPO_OPT} --od .

${HOST_EXE_DIR}/${HOST_EXE} : ${OBJECTS}
	${CC} ${HOST_LFLAGS} ${OBJECTS} -o $@ 

${XCLBIN}:
	${CLCC} -c ${CLCC_OPT} ${KERNEL_SRCS} -o $(XO)
	${CLCC} -l ${CLCC_OPT} $(XO) -o $(LINK_XCLBIN)
	${CLCC} -p $(LINK_XCLBIN) -t $(TARGET) --platform ${XDEVICE} --package.out_dir $(PACKAGE_OUT) -o $(XCLBIN)

%.o: %.cpp
	${CC} ${HOST_CFLAGS} -c $< -o $@

clean:
	${RM} -rf ${HOST_EXE} ${OBJECTS} emconfig.json .Xil *v++* *.log *.link_summary *.compile_summary _x/ *.pb *.xo

cleanall: clean
	${RM} -rf *.xclbin profile_summary.* TempConfig *.jou _vimage/ bin_vadd_cpu_emu.x* bin_vadd_hw_emu.x* bin_vadd_hw.x*


help:
	@echo "Compile and run CPU emulation using default u200_gen3x16_xdma_2_202110 XSA"
	@echo "make -f vitis.mk run_cpu_em"
	@echo ""
	@echo "Compile and run hardware emulation using default u200_gen3x16_xdma_2_202110 XSA"
	@echo "make -f vitis.mk run_hw_em"
	@echo ""
	@echo "Compile host executable only"
	@echo "make -f vitis.mk host"
	@echo ""
	@echo "Compile XCLBIN file for system run only"
	@echo "make -f vitis.mk xbin_hw"
	@echo ""
	@echo "Compile and run CPU emulation using u200_gen3x16_xdma_2_202110 XSA"
	@echo "make -f vitis.mk XDEVICE=xilinx_u200_gen3x16_xdma_2_202110 run_cpu_em"
	@echo ""
	@echo "Clean working diretory"
	@echo "make -f vitis.mk clean"
	@echo ""
	@echo "Super clean working directory"
	@echo "make -f vitis.mk cleanall"
