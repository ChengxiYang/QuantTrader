# wt > src
这是针对沪深 A 股与 ETF 交易场景裁剪后的 WonderTrader 代码结构。

## 目标范围
- 仅支持沪深股票与 ETF 交易
- 不再保留期货、期权、数字货币及其他非 A 股/ETF 品种相关代码
- 生产源码按行情接收、策略引擎、交易执行三类模块组织，回测与单元测试统一放在项目根目录的 tests 中

## 目录结构
+ ***基础库***
  - Includes、Share、WTSUtils、WTSTools：公共数据结构、接口和工具能力
+ ***行情接收***
  - market_data/WtDtCore、WtDtHelper、WtDtPorter：行情接收和转发
  - market_data/WtDataStorage：行情数据存储
+ ***策略引擎***
  - strategy_engine/WtCore：核心策略运行时
  - strategy_engine/WtCtaStraFact、WtSelStraFact：策略工厂
+ ***交易执行***
  - trade_execution/TraderXTP：沪深 A 股 / ETF 交易柜台适配
  - trade_execution/WtExeFact、WtPorter、WtRunner：执行逻辑和运行入口

## 测试单元
项目根目录的 `tests` 与 `src` 平级，统一组织回测和单元测试：

```text
tests/
  CMakeLists.txt
  backtest/
    WtBtCore/
    WtBtPorter/
    WtBtRunner/
  unit/
    CMakeLists.txt
    EventNotifierTest.cpp
    TestMsgQue.cpp
    XtpModuleTest.cpp
```

- `tests/backtest`：回测核心、动态库接口和运行入口，保持默认构建。
- `tests/unit`：自动化单元测试，由 `BUILD_TESTING` 控制并通过 CTest 运行。
- CMake 入口仍为 `src/CMakeLists.txt`，由其引入 `tests/CMakeLists.txt`；测试单元不单独配置，复用生产目标与依赖。
- 测试单元的构建中间文件位于构建目录下的 `tests`，单元测试产物位于 `tests/unit`；回测产物的原有输出路径保持不变。
- 启用 XTP 时，CTest 额外检查插件构建目录及 WtRunner、WtPorter 部署目录中的动态库加载和工厂符号，不连接柜台。

## 编译环境与 XTP SDK
- 仅支持 Linux 和 GCC/G++，使用 C++17、CMake 3.16 及以上版本；其他平台或编译器会在配置阶段报错。
- Boost 需要 1.90 及以上版本的 filesystem、thread 组件；现有 VS Code 配置使用 `build/boost-1.92.0` 中的安装。
- 所有目标统一使用顶层 PIC 设置，默认启用 `WT_WARNINGS_AS_ERRORS`。
- XTP SDK 默认放在项目根目录的 `deps` 中，包含 `xtp_trader_api.h`、其配套头文件及 Linux 动态库 `libxtptraderapi.so`。当前提供的动态库为 x86-64，必须与编译和部署架构匹配。
- `XTP_SDK_DIR` 可指定其他 SDK 目录。首次配置时，头文件和动态库均存在则默认启用 `WT_BUILD_XTP`；也可显式关闭。启用后缺少头文件或动态库会在配置阶段报错。
- 当前 VS Code 配置已显式设置 `XTP_SDK_DIR` 为根目录 `deps` 并启用 `WT_BUILD_XTP`，避免沿用旧构建缓存中的 SDK 路径或关闭状态。使用 CMake Tools 的 Configure、Build 和测试入口即可构建验证。
- 适配器保持运行时加载 SDK，不直接链接其工厂函数。构建会将 `libxtptraderapi.so` 放在 `libTraderXTP.so` 旁边，并将两者复制到 WtRunner、WtPorter 的 `traders` 目录；SDK 或插件更新会触发重新复制。部署时需要一起携带这两个动态库及其系统依赖。

## 公共头文件
- `src/Includes` 和 `src/Share` 是公共接口、数据结构与共享工具的唯一存放位置。
- 生产模块和 `tests/backtest` 下不再保留同名目录或符号链接。
- 业务分组与测试中的源码统一使用 `#include "Includes/文件名"` 或 `#include "Share/文件名"`，不要使用依赖分组内目录的 `../Includes/`、`../Share/` 路径。
- 顶层 CMake 的 `INCS` 已包含 `src` 根目录；各目标继续复用该配置，无需复制公共头文件或创建兼容链接。

## 说明
本项目已按 A 股 / ETF 交易需求裁剪，旧的期货、期权、跨品种通用框架与无关扩展模块已从当前构建入口中移除。
ParserXeleSkt 使用上期所期货行情协议，已移除；股票行情需要通过 IParserApi 接入兼容的沪深行情源。