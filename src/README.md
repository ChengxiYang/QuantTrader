# wt > src
这是针对沪深 A 股与 ETF 交易场景裁剪后的 WonderTrader 代码结构。

## 目标范围
- 仅支持沪深股票与 ETF 交易
- 不再保留期货、期权、数字货币及其他非 A 股/ETF 品种相关代码
- 按行情接收、策略引擎、交易执行、策略回测四类模块组织

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
+ ***策略回测***
  - backtest/WtBtCore、WtBtPorter、WtBtRunner：回测核心和运行入口

## 说明
本项目已按 A 股 / ETF 交易需求裁剪，旧的期货、期权、跨品种通用框架与无关扩展模块已从当前构建入口中移除。
ParserXeleSkt 使用上期所期货行情协议，已移除；股票行情需要通过 IParserApi 接入兼容的沪深行情源。