// dfx_host_compare_two_pass_opt.cpp
// Usage: ./dfx_host_compare_two_pass_opt task1.xclbin [task2.xclbin]
//
// Pass 1: cint16input -> mm2s -> polar_clip -> graph("clipgraph") -> s2mm -> output1
// Pass 2: cint16input -> mm2s -> polar_clip -> graph("clipgraph") -> s2mm -> output2
// Compare: output1 vs output2 (no golden)
//
// With a single xclbin only pass 1 runs, which is what hw_emu run/debug uses.
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
 
#include "input.h"                 // cint16input[]
#include "xrt/xrt_device.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_hw_context.h"
#include "xrt/xrt_graph.h"
#include "xrt/experimental/xrt_xclbin.h"
#include "adf/adf_api/XRTConfig.h"
 
#define INPUT_SIZE   128
#define OUTPUT_SIZE  256
#define NO_OF_ITERATIONS  4
 
// Run exactly one DFX region pass, return host output buffer.
// BOs are allocated here (after kernels) so group_id() can be used to pick
// the correct DDR memory group — group 0 in this platform is Bank Used: No.
static std::vector<int>
run_one_pass(xrt::hw_context& hw_ctx,
             int sizeInWords, int sizeOutWords,
             long itr)
{
  xrt::kernel mm2s_krnl(hw_ctx, "mm2s");
  xrt::kernel s2mm_krnl(hw_ctx, "s2mm");
  xrt::kernel polar_clip_krnl(hw_ctx, "polar_clip");
 
  xrt::run mm2s_rhdl(mm2s_krnl);
  xrt::run s2mm_rhdl(s2mm_krnl);
  xrt::run polar_clip_rhdl(polar_clip_krnl);
 
  // Allocate BOs using the memory group the kernel expects for its mem argument.
  // xclbin MEM_TOPOLOGY: group 0 = Bank Used: No; groups 1/2 = Bank Used: Yes (DDR).
  // group_id(0) returns the correct DDR group for each kernel's first argument.
  size_t in_bytes  = static_cast<size_t>(sizeInWords)  * sizeof(int);
  size_t out_bytes = static_cast<size_t>(sizeOutWords) * sizeof(int);
  xrt::bo in_bo (hw_ctx, in_bytes,  mm2s_krnl.group_id(0));
  xrt::bo out_bo(hw_ctx, out_bytes, s2mm_krnl.group_id(0));
 
  // Initialize input
  auto in_map = in_bo.map<short int*>();
  std::memcpy(in_map, cint16input, in_bytes);
 
  // Arg indices per xclbin EMBEDDED_METADATA (function order, all 4 args present):
  //   mm2s / s2mm: 0=mem(BO), 1=s(stream-skip), 2=size(scalar), 3=written(output-skip)
  //   polar_clip:  0=input(stream-skip), 1=output(stream-skip), 2=size(scalar)
  mm2s_rhdl.set_arg(0, in_bo);
  mm2s_rhdl.set_arg(2, sizeInWords);
  s2mm_rhdl.set_arg(0, out_bo);
  s2mm_rhdl.set_arg(2, sizeOutWords);
  polar_clip_rhdl.set_arg(2, sizeOutWords);
 
  // Coherency (push input)
  in_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
 
  // Open graph
  xrt::graph ghdl(hw_ctx, "clipgraph");
 
  // Start order: consumer first
  s2mm_rhdl.start();        printf("run s2mm\n");
  polar_clip_rhdl.start();  printf("run polar_clip\n");
  printf("xrtGraphRun\n");
  ghdl.run(itr);
  mm2s_rhdl.start();        printf("run mm2s\n");
 
  // Wait
  auto st = mm2s_rhdl.wait();
  std::cout << "mm2s completed with status(" << static_cast<int>(st) << ")\n";
  st = polar_clip_rhdl.wait();
  std::cout << "polar_clip completed with status(" << static_cast<int>(st) << ")\n";
  st = s2mm_rhdl.wait();
  std::cout << "s2mm completed with status(" << static_cast<int>(st) << ")\n";
 
  // Pull output before ending the graph: s2mm has already written every word,
  // so the result is in hand before any teardown step runs.
  out_bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
  auto out_ptr = out_bo.map<int*>();
  std::vector<int> host_out(sizeOutWords);
  std::memcpy(host_out.data(), out_ptr, out_bytes);
  printf("output read back\n");
 
  // Graph end
  ghdl.end(0);
  printf("xrtGraphEnd..\n");
 
  return host_out;
}
 
static int
compare_outputs(const std::vector<int>& out1, const std::vector<int>& out2)
{
  if (out1.size() != out2.size()) {
    std::cerr << "Size mismatch: out1=" << out1.size() << " out2=" << out2.size() << "\n";
    return 1;
  }
 
  int errCnt = 0;
  for (size_t i = 0; i < out1.size(); ++i) {
    if (out1[i] != out2[i]) {
      if (errCnt < 16)
        printf("DIFF @%zu: out1=0x%08x  out2=0x%08x\n", i, out1[i], out2[i]);
      ++errCnt;
    }
  }
  return errCnt;
}
 
// The IDE starts run and debug from the same launch configuration, so the
// environment is identical in both; what differs is that a debug session
// ptrace-attaches an agent, which shows up as a non-zero TracerPid.
static bool
under_debugger()
{
  std::ifstream status("/proc/self/status");
  std::string line;
  while (std::getline(status, line)) {
    if (line.compare(0, 10, "TracerPid:") == 0)
      return std::stoi(line.substr(10)) != 0;
  }
  return false;
}
 
// Must be called while the XRT handles are still open: closing the hw_context
// and the device unloads the partition and flushes profile/trace data, which
// under hw_emu with a debugger attached can take very long or stall.
static void
report(int errCnt)
{
  std::cout << (under_debugger() ? "DEBUG" : "RUN")
<< (errCnt ? " FAILED" : " PASSED")
<< " (mismatches=" << errCnt << ")\n";
}
 
int main(int argc, char ** argv)
{
  // Under a debugger stdout is a pipe and therefore fully buffered. Make every
  // message appear as it is produced, so the result is on the console even if
  // the process later stalls while closing the device.
  setvbuf(stdout, nullptr, _IONBF, 0);
  std::cout << std::unitbuf;
 
  try {
    if (argc < 2) {
      std::cout << "Usage: " << argv[0] <<" <task1.xclbin> [task2.xclbin]\n";
      return EXIT_FAILURE;
    }
 
    const char* xclbin1 = argv[1];
    const char* xclbin2 = (argc > 2) ? argv[2] : nullptr;
 
    const long itr         = NO_OF_ITERATIONS;
    const int  sizeInWords  = INPUT_SIZE  * itr;
    const int  sizeOutWords = OUTPUT_SIZE * itr;
 
    xrt::device device(0);
 
    // ---------- PASS 1 ----------
    std::vector<int> out1;
    {
      xrt::xclbin xclbin1_obj{std::string{xclbin1}};
      auto uuid1 = device.register_xclbin(xclbin1_obj);
      xrt::hw_context hw_ctx1(device, uuid1);
 
      std::cout << "[PASS 1] load " << xclbin1 << "\n";
      out1 = run_one_pass(hw_ctx1, sizeInWords, sizeOutWords, itr);
 
      if (!xclbin2) {
        // Single xclbin: no second output to compare against, so completing
        // pass 1 is the test. Reported here, before hw_ctx1 goes away.
        for (size_t i = 0; i < out1.size() && i < 16; ++i)
          printf("out1[%zu] = 0x%08x\n", i, out1[i]);
        std::cout << "[PASS 1] " << out1.size() << " words from " << xclbin1 << "\n";
        report(0);
        return EXIT_SUCCESS;
      }
    } // hw_ctx1 destroyed (RAII) before pass 2 xclbin load
 
    // ---------- PASS 2 ----------
    int errCnt = 0;
    {
      xrt::xclbin xclbin2_obj{std::string{xclbin2}};
      auto uuid2 = device.register_xclbin(xclbin2_obj);
      xrt::hw_context hw_ctx2(device, uuid2);
 
      std::cout << "[PASS 2] load " << xclbin2 << "\n";
      std::vector<int> out2 = run_one_pass(hw_ctx2, sizeInWords, sizeOutWords, itr);
 
      errCnt = compare_outputs(out1, out2);
      report(errCnt);
    } // hw_ctx2 destroyed (RAII)
 
    return errCnt ? EXIT_FAILURE : EXIT_SUCCESS;
  }
  catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return EXIT_FAILURE;
  }
}