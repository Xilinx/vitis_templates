<table width="100%">
 <tr width="100%">
    <td align="center"><img src="https://raw.githubusercontent.com/Xilinx/Image-Collateral/main/xilinx-logo.png" width="30%"/><h1>AMD Vitis™ AI Engine and AI Engine System Design Examples</h1>
    </td>
 </tr>
</table>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at [Apache Licence, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.

Welcome to the Vitis Examples repository! This repository provides a collection of examples designed to showcase the capabilities of the Vitis™ tools, targeting AI Engine and XRT applications.

These examples demonstrate specific scenarios involving host code and kernel programming through concise, functional implementations. They are intended to serve as a reference, helping users develop their own accelerated applications on AMD Accelerated Platforms.

Note: Users can also utilize custom platforms that support the devices in both GUI-based and Makefile-based applications.

## Examples

### `aie_examples` — AIE graph templates (emulation only)

| Example | Description |
|---|---|
| [`simple`](aie_examples/simple/) | 2-kernel graph with window-based data communication |
| [`simple_chained`](aie_examples/simple_chained/) | 2-kernel graph with array parameter triggered communication |
| [`simple_single_buffer`](aie_examples/simple_single_buffer/) | Single buffer with margin copy |
| [`simple_128_bit_interface`](aie_examples/simple_128_bit_interface/) | 128-bit downsizer/upsizer ME–PL interface |
| [`simple_64_bit_interface`](aie_examples/simple_64_bit_interface/) | 64-bit downsizer/upsizer ME–PL interface |
| [`simple_bypass`](aie_examples/simple_bypass/) | Bypass demonstration |
| [`simple_margin`](aie_examples/simple_margin/) | Margin copy demonstration |
| [`simple_param`](aie_examples/simple_param/) | Scalar parameter update via external trigger |
| [`simple_packet_split_merge`](aie_examples/simple_packet_split_merge/) | Packet stream split and merge |
| [`single_node_template`](aie_examples/single_node_template/) | Window (single/double buffer), stream, and RTP array connections |
| [`template_window_class`](aie_examples/template_window_class/) | C++ templated data types and state encapsulation |
| [`async_buffer`](aie_examples/async_buffer/) | ADF async windows |
| [`async_rtp_control_iterative`](aie_examples/async_rtp_control_iterative/) | Asynchronous RTP update with iteration control |
| [`GMIO_bandwidth`](aie_examples/GMIO_bandwidth/) | GMIO performance profiling |
| [`MappingPlacement`](aie_examples/MappingPlacement/) | Relocatable mapping as template parameters |
| [`ShimConstraints`](aie_examples/ShimConstraints/) | Physical channel allocation constraints on AIE–PL boundary |
| [`StreamSwitchFIFO`](aie_examples/StreamSwitchFIFO/) | Stream Switch FIFO to avoid deadlocks |

### `aie_system_examples` — Full AIE + PL + PS system designs

| Example | Target Board | Description |
|---|---|---|
| [`aie_sys_design`](aie_system_examples/aie_sys_design/) | VCK190 | FIR filter + polar-clip + classifier pipeline |
| [`aie-ml_sys_design`](aie_system_examples/aie-ml_sys_design/) | VEK280 / VEK385 | 8-bit matrix multiply on AIE-ML |
| [`aie_dfx_design`](aie_system_examples/aie_dfx_design/) | VCK190 | Dynamic Function Exchange (DFX) with two swappable AIE+PL regions |

### `vadd` — PL-only vector addition

| Example | Target Board | Description |
|---|---|---|
| [`vadd`](vadd/) | VCK190 | Vector addition using XRT Native API + HLS dataflow kernel |

<p class="sphinxhide" align="center"><sub>Copyright © 2020–2026 Advanced Micro Devices, Inc</sub></p>

<p class="sphinxhide" align="center"><sup><a href="https://www.amd.com/en/corporate/copyright">Terms and Conditions</a></sup></p>
