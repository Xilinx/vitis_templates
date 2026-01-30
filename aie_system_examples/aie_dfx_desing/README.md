
###  DFX Design development Flow
**Vitis 2025.1**

```mermaid
flowchart LR
    %% 平台
    PFM[VCK190 DFX XPFM <br/> DTB, BOOT.bin self contained]

    %% 左侧 DR1
    subgraph DR1[Dynamic Region 1]
      direction TB
      HLS1[HLS Kernels 1]
      AIE1[AIE Graph 1]
      XSA1[Fixed_XSA: DR1.xsa<br/>v++ -l]
      XCL1[XCLBIN1<br/>v++ -p defer_aie_run]
    end

    %% 右侧 DR2
    subgraph DR2[Dynamic Region 2]
      direction TB
      HLS2[HLS Kernels 2]
      AIE2[AIE Graph 2]
      XSA2[Fixed_XSA: DR2.xsa<br/>v++ -l]
      XCL2[XCLBIN2<br/>v++ -p defer_aie_run]
    end

    %% SD卡镜像输入
    ROOTFS[Rootfs]
    IMAGE[Linux Image]
    HOST[HOST Application]

    %% 输出
    SD[SD_card.img<br/>v++ -p package]

    %% —— 关键：用隐形锚点把 PFM 分到左右 ——
    PFM --> PFM_L
    PFM --> PFM_R
    PFM_L --> XSA1
    PFM_R --> XSA2
    %% 隐形样式
    style PFM_L fill:transparent,stroke:transparent
    style PFM_R fill:transparent,stroke:transparent

    %% DR1 连接（不与 PFM 交叉）
    HLS1 --> XSA1
    AIE1 --> XSA1
    XSA1 --> XCL1
    AIE1 --> XCL1

    %% DR2 连接
    HLS2 --> XSA2
    AIE2 --> XSA2
    XSA2 --> XCL2
    AIE2 --> XCL2

    %% 打包
    XCL1 --> SD
    XCL2 --> SD
    ROOTFS --> SD
    IMAGE --> SD
    HOST  --> SD
    PFM   --> SD

    %% 样式
    classDef platform fill:#e8f6f3,color:#2c3e50,stroke:#1abc9c
    classDef aie fill:#fef9e7,color:#2c3e50,stroke:#f39c12
    classDef hls fill:#f4ecf7,color:#2c3e50,stroke:#8e44ad
    classDef xsa fill:#fdebd0,color:#2c3e50,stroke:#e67e22
    classDef xclbin fill:#d5f5e3,color:#2c3e50,stroke:#27ae60
    classDef sd_components fill:#f4ecf7,color:#2c3e50,stroke:#8e44ad
    classDef sd_image fill:#e8daef,color:#2c3e50,stroke:#7d3c98

    class PFM platform
    class AIE1,AIE2 aie
    class HLS1,HLS2 hls
    class XSA1,XSA2 xsa
    class XCL1,XCL2 xclbin
    class ROOTFS,IMAGE,HOST sd_components
    class SD sd_image
```

>Note: In this diagram, PFM_L and PFM_R represent the same vck190 base DFX platform as XPFM — they are just shown separately to make the diagram lines clearer.
