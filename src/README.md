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
```

- `tests/backtest`：回测核心、动态库接口和运行入口，保持默认构建。
- `tests/unit`：自动化单元测试，由 `BUILD_TESTING` 控制并通过 CTest 运行。
- CMake 入口仍为 `src/CMakeLists.txt`，由其引入 `tests/CMakeLists.txt`；测试单元不单独配置，复用生产目标与依赖。
- 测试单元的构建中间文件位于构建目录下的 `tests`，单元测试产物位于 `tests/unit`；回测产物的原有输出路径保持不变。

## 公共头文件
- `src/Includes` 和 `src/Share` 是公共接口、数据结构与共享工具的唯一存放位置。
- 生产模块和 `tests/backtest` 下不再保留同名目录或符号链接。
- 业务分组与测试中的源码统一使用 `#include "Includes/文件名"` 或 `#include "Share/文件名"`，不要使用依赖分组内目录的 `../Includes/`、`../Share/` 路径。
- 顶层 CMake 的 `INCS` 已包含 `src` 根目录；各目标继续复用该配置，无需复制公共头文件或创建兼容链接。

## 说明
本项目已按 A 股 / ETF 交易需求裁剪，旧的期货、期权、跨品种通用框架与无关扩展模块已从当前构建入口中移除。
ParserXeleSkt 使用上期所期货行情协议，已移除；股票行情需要通过 IParserApi 接入兼容的沪深行情源。