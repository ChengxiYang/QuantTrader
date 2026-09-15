# wt > src
这是针对沪深 A 股与 ETF 交易场景裁剪后的 WonderTrader 代码结构。

## 目标范围
- 仅支持沪深股票与 ETF 交易
- 不再保留期货、期权、数字货币及其他非 A 股/ETF 品种相关代码
- 生产源码按行情接收、策略引擎、交易执行三类模块组织，回测与单元测试统一放在项目根目录的 tests 中

## 目录结构
+ ***程序入口***
  - [src/main.cpp](src/main.cpp)：WtRunner 实盘运行程序的入口
+ ***基础库***
  - Includes、Share、WTSUtils、WTSTools：公共数据结构、接口和工具能力
+ ***行情接收***
  - market_data/WtDtCore、WtDtHelper、WtDtPorter：行情接收和转发
  - market_data/WtDataStorage：行情数据存储
+ ***策略引擎***
  - strategy_engine/WtCore：共享引擎、数据管理、适配器及执行设施
  - strategy_engine/cta：CTA 运行时、WtCtaStraFact 工厂及 DualThrust 示例
  - strategy_engine/hft：HFT 事件驱动运行框架，暂无具体示例策略
  - strategy_engine/sel：SEL 运行时、WtSelStraFact 工厂及 DualThrustSelection 示例
  - [策略说明](STRATEGIES.md)：实际类别、算法逻辑、参数、加载方式及既有限制
+ ***交易执行***
  - trade_execution/TraderXTP：沪深 A 股 / ETF 交易柜台适配
  - trade_execution/WtExeFact、WtPorter、WtRunner：执行逻辑、动态库接口和运行编排

## 启动入口与流程
[src/main.cpp](src/main.cpp) 是生产可执行程序 `WtRunner` 的入口，并不是所有构建目标共用的入口。回测程序的入口在 [tests/backtest/WtBtRunner/WtBtRunner.cpp](tests/backtest/WtBtRunner/WtBtRunner.cpp)，单元测试也有独立入口。行情、策略、交易模块通过库和插件协作，不是依次启动的独立进程。

1. `main()` 解析 `-l/--logcfg`、`-c/--config`、`-h/--help`；默认读取当前工作目录下的 `logcfg.yaml` 和 `config.yaml`，帮助模式输出参数说明后直接退出。
2. 构造 `WtRunner`，安装信号处理；`init()` 初始化日志，并将启动工作目录设置为实例目录。
3. `config()` 加载主配置和基础数据，随后依次初始化引擎、数据管理、交易动作策略、行情适配器、交易适配器、事件通知器、非 HFT 执行器及路由，最后加载策略工厂并创建策略上下文。引擎选择由 `env.name` 控制。主配置加载失败或交易动作策略初始化失败时，`main()` 返回 1；并非所有组件初始化失败都会向上传播。
4. `run(false)` 依次执行 `_parsers.run()`、`_traders.run()`、`_engine->run()`。前两者发起行情和交易连接，并不表示连接或登录已同步完成；引擎启动相应的行情时钟及调度逻辑。
5. 运行期间，行情进入策略引擎；CTA 策略产生目标仓位，经路由和执行器交给交易适配器，委托与成交回报再反馈给执行器和策略。HFT 策略上下文直接绑定交易适配器。主线程每 10 毫秒检查退出标志，收到停止信号后离开等待循环并返回。

入口迁移不改变 `WtRunner` 目标名或输出目录。当前 Debug 构建的程序位于 `build/build_x64/Debug/bin/WtRunner/WtRunner`，仍由 [src/trade_execution/WtRunner/CMakeLists.txt](src/trade_execution/WtRunner/CMakeLists.txt) 显式编译根目录入口文件。

运行时需准备完整配置、基础数据、策略插件及行情和交易模块。相对配置路径及 `cta/`、`hft/`、`executer/` 等插件目录依赖进程工作目录；指定 `-c` 不会自动切换到配置文件所在目录。当前仓库的 `config/` 为空，沪深行情源仍需单独接入，构建成功不等于已具备实盘运行条件。

现有 `SEL` 分支仅完成引擎选择，`WtRunner::config()` 的非 HFT 策略初始化及执行器、路由绑定仍使用 CTA 路径，不能据此认为 SEL 启动链已经完整接通。本次入口迁移不修改这些既有行为。

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