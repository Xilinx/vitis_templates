#/*
#Copyright (C) 2026, Advanced Micro Devices, Inc.
#SPDX-License-Identifier: MIT
#*/

ROOT_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
PLATFORM  = /proj/rdi/xbuilds/2025.1_released/internal_platforms/xilinx_vck190_base_dfx_202510_1/xilinx_vck190_base_dfx_202510_1.xpfm
VERSION  := 2025.1
COMMON_IMAGE_VERSAL ?= ${PLATFORM_REPO_PATHS}/sw/versal/xilinx-versal-common-v$(VERSION)/
SYSROOT := $(COMMON_IMAGE_VERSAL)/sysroots/cortexa72-cortexa53-amd-linux/

# === Put ALL build artifacts under this temp root ===
TMP_ROOT ?= $(ROOT_DIR)/tmp

.PHONY: help
help:
	@echo "Makefile usage"
	@echo "  make build_xsa     ## build both task1.xsa and task2.xsa into tmp/"
	@echo "  make build_xclbin  ## build both task1.xclbin and task2.xclbin into tmp/"
	@echo "  make clean         ## remove the tmp/ directory"

TARGET ?= hw
ifeq ($(TARGET),sw_emu)
  TASK1_AIE_FLAGS += --target=x86sim -Xpreproc='-DNDEBUG'
  TASK2_AIE_FLAGS += --target=x86sim -Xpreproc='-DNDEBUG'
else
  TASK1_AIE_FLAGS += --target=hw
  TASK2_AIE_FLAGS += --target=hw
endif

# ----------------------------- Task 1 paths -----------------------------
TASK1_DIR           := $(ROOT_DIR)/task1
TASK1_BUILD_DIR     := $(TMP_ROOT)/task1
TASK1_GRAPH         := $(TASK1_DIR)/aie/graph.cpp
TASK1_AIE_WORK      := $(TASK1_BUILD_DIR)/Work
TASK1_GRAPH_O       := $(TASK1_BUILD_DIR)/libadf.a
TASK1_LINK_OUTPUT   := $(TASK1_BUILD_DIR)/task1.xsa
TASK1_XCLBIN        := $(TASK1_BUILD_DIR)/task1.xclbin
TASK1_LINK_CONFIG   := $(TASK1_DIR)/system.cfg
TASK1_KERNEL_SRCS   := $(TASK1_DIR)/hls/s2mm.cpp $(TASK1_DIR)/hls/mm2s.cpp $(TASK1_DIR)/hls/polar_clip.cpp
TASK1_KERNEL_STEMS  := $(basename $(notdir $(TASK1_KERNEL_SRCS)))
TASK1_KERNEL_XO     := $(addprefix $(TASK1_BUILD_DIR)/,$(addsuffix .xo,$(TASK1_KERNEL_STEMS)))

# ----------------------------- Task 2 paths -----------------------------
TASK2_DIR           := $(ROOT_DIR)/task2
TASK2_BUILD_DIR     := $(TMP_ROOT)/task2
TASK2_GRAPH         := $(TASK2_DIR)/aie/graph.cpp
TASK2_AIE_WORK      := $(TASK2_BUILD_DIR)/Work
TASK2_GRAPH_O       := $(TASK2_BUILD_DIR)/libadf.a
TASK2_LINK_OUTPUT   := $(TASK2_BUILD_DIR)/task2.xsa
TASK2_XCLBIN        := $(TASK2_BUILD_DIR)/task2.xclbin
TASK2_LINK_CONFIG   := $(TASK2_DIR)/system.cfg
TASK2_KERNEL_SRCS   := $(TASK2_DIR)/hls/s2mm.cpp $(TASK2_DIR)/hls/mm2s.cpp $(TASK2_DIR)/hls/polar_clip.cpp
TASK2_KERNEL_STEMS  := $(basename $(notdir $(TASK2_KERNEL_SRCS)))
TASK2_KERNEL_XO     := $(addprefix $(TASK2_BUILD_DIR)/,$(addsuffix .xo,$(TASK2_KERNEL_STEMS)))

# Tools
VPP    := v++
AIECC  := aiecompiler
AIESIM := aiesimulator

# Common include flags for AIE
TASK1_AIE_INCLUDE_FLAGS := -include="$(XILINX_VITIS)/aietools/include" \
                           -include="$(TASK1_DIR)/aie" \
                           -include="$(TASK1_DIR)/aie/kernels" \
                           -include="$(TASK1_DIR)/data" -include="./"

TASK2_AIE_INCLUDE_FLAGS := -include="$(XILINX_VITIS)/aietools/include" \
                           -include="$(TASK2_DIR)/aie" \
                           -include="$(TASK2_DIR)/aie/kernels" \
                           -include="$(TASK2_DIR)/data" -include="./"

TASK1_AIE_FLAGS := --platform $(PLATFORM) $(TASK1_AIE_INCLUDE_FLAGS) --pl-freq=100 -workdir=$(TASK1_AIE_WORK)
TASK2_AIE_FLAGS := --platform $(PLATFORM) $(TASK2_AIE_INCLUDE_FLAGS) --pl-freq=100 -workdir=$(TASK2_AIE_WORK)

#host code
HOST_SRCS := ./host/host.cpp
EXECUTABLE = ./double_lab8

GCC_FLAGS := -Wall -c \
			 -Wno-int-to-pointer-cast \
			 --sysroot=$(SYSROOT) -Wall -Wextra -O0 -g3 -std=c++17 -MD -MT

GCC_INCLUDES := -I${XILINX_VITIS}/include \
				-I$(SYSROOT)/usr/include/xrt \
				-I./ -I./src/ \
				-I${XILINX_VITIS}/aietools/include \
				-I${XILINX_VITIS}/include
ifeq ($(EMU_PS),X86)
CXX := g++
else
CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
endif
GCC_LIB := -lxrt_core -lxrt_coreutil \
		   -L$(SYSROOT)/usr/lib \
		   --sysroot=$(SYSROOT) \
		   -L${XILINX_VITIS}/aietools/lib/aarch64.o

ifeq ($(TARGET),$(filter $(TARGET),sw_emu))
ifeq ($(EMU_PS), X86)
       GCC_LIB := -lxrt_coreutil -L${XILINX_VITIS}/aietools/lib/lnx64.o -L${XILINX_XRT}/lib
       GCC_FLAGS := -Wall -c -std=c++17 -Wno-int-to-pointer-cast -I${XILINX_XRT}/include
       GCC_INCLUDES := -I./src/ -I./ -I${XILINX_VITIS}/aietools/include
else
       GCC_LIB := -lxrt_coreutil -L$(SYSROOT)/usr/lib --sysroot=$(SYSROOT) -L${XILINX_VITIS}/aietools/lib/aarch64.o
endif
endif
# Helper to ensure per-task temp folders exist
$(TASK1_BUILD_DIR)/.dirs:
	@mkdir -p $(TASK1_BUILD_DIR)/_x $(TASK1_BUILD_DIR)/logs $(TASK1_BUILD_DIR)/reports
	@mkdir -p $(TASK1_AIE_WORK)
	@touch $@

$(TASK2_BUILD_DIR)/.dirs:
	@mkdir -p $(TASK2_BUILD_DIR)/_x $(TASK2_BUILD_DIR)/logs $(TASK2_BUILD_DIR)/reports
	@mkdir -p $(TASK2_AIE_WORK)
	@touch $@

# ============================ HLS COMPILE (XO) ============================
.PHONY: task1_xo task2_xo

task1_xo: $(TASK1_KERNEL_XO)
$(TASK1_BUILD_DIR)/%.xo: $(TASK1_DIR)/hls/%.cpp | $(TASK1_BUILD_DIR)/.dirs
	$(VPP) -c -t $(TARGET) --platform $(PLATFORM) -k $(basename $(notdir $<)) $< \
	  --temp_dir $(TASK1_BUILD_DIR)/_x --log_dir $(TASK1_BUILD_DIR)/logs --report_dir $(TASK1_BUILD_DIR)/reports \
	  --save-temps -g -o $@

task2_xo: $(TASK2_KERNEL_XO)
$(TASK2_BUILD_DIR)/%.xo: $(TASK2_DIR)/hls/%.cpp | $(TASK2_BUILD_DIR)/.dirs
	$(VPP) -c -t $(TARGET) --platform $(PLATFORM) -k $(basename $(notdir $<)) $< \
	  --temp_dir $(TASK2_BUILD_DIR)/_x --log_dir $(TASK2_BUILD_DIR)/logs --report_dir $(TASK2_BUILD_DIR)/reports \
	  --save-temps -g -o $@

# ============================ AIE BUILD (libadf.a) ============================
.PHONY: task1_graph task2_graph

task1_graph: $(TASK1_GRAPH_O)
$(TASK1_GRAPH_O): $(TASK1_GRAPH) | $(TASK1_BUILD_DIR)/.dirs
	cd $(TASK1_BUILD_DIR) && $(AIECC) $(TASK1_AIE_FLAGS) $(abspath $<)
	@echo "COMPLETE: $(TASK1_GRAPH_O)"

task2_graph: $(TASK2_GRAPH_O)
$(TASK2_GRAPH_O): $(TASK2_GRAPH) | $(TASK2_BUILD_DIR)/.dirs
	cd $(TASK2_BUILD_DIR) && $(AIECC) $(TASK2_AIE_FLAGS) $(abspath $<)
	@echo "COMPLETE: $(TASK2_GRAPH_O)"

# ============================ LINK (XSA) ============================
$(TASK1_LINK_OUTPUT): $(TASK1_KERNEL_XO) $(TASK1_GRAPH_O)
	$(VPP) -l --platform $(PLATFORM) $(TASK1_KERNEL_XO) $(TASK1_GRAPH_O) -t $(TARGET) \
	  --config $(TASK1_LINK_CONFIG) \
	  --temp_dir $(TASK1_BUILD_DIR)/_x --log_dir $(TASK1_BUILD_DIR)/logs --report_dir $(TASK1_BUILD_DIR)/reports \
	  --save-temps -g -o $@
	@echo "COMPLETE: $@"

$(TASK2_LINK_OUTPUT): $(TASK2_KERNEL_XO) $(TASK2_GRAPH_O)
	$(VPP) -l --platform $(PLATFORM) $(TASK2_KERNEL_XO) $(TASK2_GRAPH_O) -t $(TARGET) \
	  --config $(TASK2_LINK_CONFIG) \
	  --temp_dir $(TASK2_BUILD_DIR)/_x --log_dir $(TASK2_BUILD_DIR)/logs --report_dir $(TASK2_BUILD_DIR)/reports \
	  --save-temps -g -o $@
	@echo "COMPLETE: $@"

.PHONY: build_xsa
build_xsa: $(TASK1_LINK_OUTPUT) $(TASK2_LINK_OUTPUT)

# ============================ XCLBIN (link to xclbin) ============================
$(TASK1_XCLBIN): $(TASK1_LINK_OUTPUT)
	$(VPP) -p --platform $(PLATFORM) -t $(TARGET) $(TASK1_GRAPH_O) $(TASK1_LINK_OUTPUT) \
	  --temp_dir $(TASK1_BUILD_DIR)/_x --log_dir $(TASK1_BUILD_DIR)/logs --report_dir $(TASK1_BUILD_DIR)/reports \
	  --package.defer_aie_run \
	  --save-temps -g -o $@

$(TASK2_XCLBIN): $(TASK2_LINK_OUTPUT)
	$(VPP) -p --platform $(PLATFORM) -t $(TARGET) $(TASK2_GRAPH_O) $(TASK2_LINK_OUTPUT) \
	  --temp_dir $(TASK2_BUILD_DIR)/_x --log_dir $(TASK2_BUILD_DIR)/logs --report_dir $(TASK2_BUILD_DIR)/reports \
	  --package.defer_aie_run \
	  --save-temps -g -o $@

.PHONY: build_xclbin
build_xclbin: $(TASK1_XCLBIN) $(TASK2_XCLBIN)

#=======compile the host.
host: $(EXECUTABLE)

$(EXECUTABLE): $(HOST_SRCS) 
ifneq ($(EMU_PS), X86)
endif 
		$(CXX) $(HOST_SRCS) $(GCC_FLAGS) $(GCC_INCLUDES) -o main.o
		$(CXX) *.o $(GCC_LIB) -o $(EXECUTABLE)
	@echo "COMPLETE: Host application created."

# ============================ PACKAGING (optional) ============================
RUN_APP_SCRIPT=$(ROOT_DIR)/run_aie.sh
PACKAGE_OUT=$(TMP_ROOT)/package_$(TARGET)
SD_CARD =sd_card.img
sd_card: $(SD_CARD)
$(SD_CARD):$(EXECUTABLE) $(TASK2_XCLBIN) $(TASK1_XCLBIN)
	$(VPP) -p  --platform $(PLATFORM) -t $(TARGET) \
	--package.out_dir $(PACKAGE_OUT) \
	--package.rootfs $(COMMON_IMAGE_VERSAL)/rootfs.ext4 \
	--package.image_format=ext4 \
	--package.boot_mode=sd \
	--package.kernel_image=$(COMMON_IMAGE_VERSAL)/Image \
	--package.sd_file $(RUN_APP_SCRIPT) \
	--package.sd_file $(TASK1_XCLBIN) \
	--package.sd_file $(TASK2_XCLBIN) \
	--package.sd_file $(EXECUTABLE)

# ============================ CLEAN ============================
.PHONY: clean ultraclean
clean:
	rm -rf $(TMP_ROOT)

ultraclean: clean
	@echo "Workspace cleaned (all outputs lived under tmp/)."
