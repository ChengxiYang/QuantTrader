# 策略说明

本文基于当前源码，区分策略运行框架、具体示例算法和订单执行算法。项目面向沪深 A 股与 ETF；保留的通用接口及做空分支不代表已提供其他市场的完整交易支持。

## 1. 当前策略类别

| 类别 | 驱动方式与职责 | 当前实现 |
| --- | --- | --- |
| CTA | 以主 K 线闭合触发计算，支持开平仓、目标仓位和条件单，交由执行器处理实际委托 | 引擎、上下文、管理器，以及 `DualThrust` 示例和工厂 |
| HFT | tick、逐笔委托、逐笔成交和交易回报驱动，上下文直接绑定交易适配器进行买卖、撤单 | 引擎、上下文和管理器；没有具体示例策略或 HFT 工厂实现 |
| SEL | 定时调度标的池，提交各标的目标仓位 | 引擎、上下文、管理器，以及 `DualThrustSelection` 示例和工厂；实盘配置启动链尚未完整接通 |

因此，当前 `strategy_engine` 包含 **3 类运行框架、2 个具体策略示例**，不是三种已经可以直接运行的交易算法。

另有 [UftStrategyDefs.h](src/Includes/UftStrategyDefs.h)、[IUftStraCtx.h](src/Includes/IUftStraCtx.h) 和回测 [UftMocker.cpp](tests/backtest/WtBtCore/UftMocker.cpp)，但 `strategy_engine` 没有对应实盘引擎或示例工厂，不为其建立空目录。

TWAP、VWAP、最小冲击及股票、差量执行变体由 [WtExeFact.cpp](src/trade_execution/WtExeFact/WtExeFact.cpp) 注册，属于订单执行算法，继续保留在交易执行模块，不计入上述信号策略示例。

## 2. 目录与模块边界

```text
QuantTrader/
  STRATEGIES.md
  src/
    strategy_engine/
      WtCore/                       共享运行设施，保留 WtCore 静态库目标
      cta/
        CtaStrategyMgr.cpp/.h
        CtaStraBaseCtx.cpp/.h
        CtaStraContext.cpp/.h
        WtCtaEngine.cpp/.h
        WtCtaTicker.cpp/.h
        WtCtaStraFact/               CTA 工厂及 DualThrust 示例
      hft/
        HftStrategyMgr.cpp/.h
        HftStraBaseCtx.cpp/.h
        HftStraContext.cpp/.h
        WtHftEngine.cpp/.h
        WtHftTicker.cpp/.h
      sel/
        SelStrategyMgr.cpp/.h
        SelStraBaseCtx.cpp/.h
        SelStraContext.cpp/.h
        WtSelEngine.cpp/.h
        WtSelTicker.cpp/.h
        WtSelStraFact/               SEL 工厂及 DualThrustSelection 示例
    Includes/                       公共策略接口，位置不变
    market_data/                    行情模块，位置不变
    trade_execution/                执行器、适配器和运行入口，位置不变
  tests/                            回测与单元测试，位置不变
```

- `cta/`、`hft/`、`sel/` 各自包含该类的引擎、时钟、上下文、策略管理器及已有工厂。
- `WtCore/` 保留公共引擎 `WtEngine`、数据管理 `WtDtMgr`、行情/交易适配器、执行器及其管理器、事件通知、过滤和辅助设施。它们被多类策略复用，不归入单一策略目录。
- 公共接口仍在 [src/Includes](src/Includes)，回测仍在 [tests/backtest](tests/backtest)，不复制接口、不建立兼容链接，也不另建一套项目结构。
- 这是按类别整理源码，不是拆分库或修改 ABI。[WtCore 构建脚本](src/strategy_engine/WtCore/CMakeLists.txt) 收集共享目录和三类目录的运行时源码，仍生成一个 `WtCore` 静态库；两个工厂仍分别生成 `WtCtaStraFact`、`WtSelStraFact` 动态库。
- 构建入口仍为 [src/CMakeLists.txt](src/CMakeLists.txt)，目标名、插件导出名及输出目录不变。源码中的 `cta/`、`hft/` 目录与运行工作目录下的同名插件部署目录不是同一个目录。

## 3. CTA：DualThrust

### 代码入口

- 策略：[WtStraDualThrust.cpp](src/strategy_engine/cta/WtCtaStraFact/WtStraDualThrust.cpp)，类 `WtStraDualThrust`，继承 `CtaStrategy`。
- 工厂：[WtCtaStraFact.cpp](src/strategy_engine/cta/WtCtaStraFact/WtCtaStraFact.cpp)，工厂名 `WtCtaStraFact`，策略名 `DualThrust`。
- 接口：[CtaStrategyDefs.h](src/Includes/CtaStrategyDefs.h)、[ICtaStraCtx.h](src/Includes/ICtaStraCtx.h)。
- 运行链：`WtCtaEngine/WtCtaRtTicker` → `CtaStraBaseCtx::on_schedule` → `CtaStraContext::on_calculate` → 策略 `on_schedule`。

### 计算与交易规则

初始化订阅单个 `code` 的 tick，并用 `stra_get_bars(..., true)` 指定主 K 线。示例 `on_tick` 不计算信号，交易判断发生在调度回调中。

令历史窗口的最高价、最低价、最高收盘价、最低收盘价分别为 $HH$、$LL$、$HC$、$LC$，最新 K 线开盘价为 $O$：

$$
R = \max(HH-LC, HC-LL),\qquad U=O+k_1R,\qquad L=O-k_2R
$$

历史窗口实际使用索引 `[-days, -2]`，排除最新 K 线；当数据充足且 `days >= 2` 时包含 `days - 1` 根 K 线。`days` 不是自动按交易日取数，具体时间跨度取决于 `period`。索引规则见 [WTSDataDef.hpp](src/Includes/WTSDataDef.hpp)。

以最新 K 线的**收盘价**判断突破：

| 当前仓位 | 条件 | 请求动作 |
| --- | --- | --- |
| 空仓 | 收盘价大于等于上轨 | 开多 `2 * trdUnit` |
| 空仓 | 收盘价小于等于下轨，且非股票模式 | 开空 `2 * trdUnit` |
| 多仓 | 收盘价小于等于下轨 | 平多 `2 * trdUnit` |
| 空头仓位 | 收盘价大于等于上轨，且非股票模式 | 平空 `2 * trdUnit` |

`stock=true` 时 `trdUnit=100`，示例数量为 200，且禁止开空；否则单位为 1。单次回调只执行一个分支，不会先平仓再立即反手。实际成交及可平数量由上下文、执行器和交易适配器处理，信号不等于成交。

示例还注册上下轨指标和买卖图表标记；在 `on_session_begin` 解析真实代码，代码变化时包含迁移旧代码持仓的通用逻辑。该逻辑的保留不改变本项目的 A 股/ETF 范围。

## 4. HFT：事件驱动框架

代码集中于 [src/strategy_engine/hft](src/strategy_engine/hft)，公共接口为 [HftStrategyDefs.h](src/Includes/HftStrategyDefs.h) 和 [IHftStraCtx.h](src/Includes/IHftStraCtx.h)。

- `WtHftEngine/WtHftRtTicker` 处理行情驱动和时间推进。
- `HftStrategyMgr` 加载外部工厂并创建策略。
- `HftStraContext` 转发 tick、逐笔数据及订单、成交等事件给策略。
- `HftStraBaseCtx` 提供查询、买入、卖出和撤单能力，直接调用绑定的 `TraderAdapter`，不同于 CTA 的目标仓位执行路径。

当前没有继承 `HftStrategy` 的具体示例，也没有可配置即运行的 HFT 算法名称或默认参数。使用前需要自行实现工厂与策略、接入行情和交易模块；存在框架不等于已验证性能、延迟或实盘可用性。

## 5. SEL：DualThrustSelection

### 代码入口

- 策略：[WtStraDtSel.cpp](src/strategy_engine/sel/WtSelStraFact/WtStraDtSel.cpp)，类 `WtStraDtSel`，继承 `SelStrategy`。
- 工厂：[WtSelStraFact.cpp](src/strategy_engine/sel/WtSelStraFact/WtSelStraFact.cpp)，工厂名 `WtSelStraFact`，策略名 `DualThrustSelection`。
- 接口：[SelStrategyDefs.h](src/Includes/SelStrategyDefs.h)、[ISelStraCtx.h](src/Includes/ISelStraCtx.h)。
- 运行链：`WtSelEngine::on_minute_end` 匹配任务 → 上下文调度 → 策略 `on_schedule`。这描述代码组织，当前实盘链路的限制见下文。

### 与 CTA 示例的区别

`codes` 是逗号分隔的代码池，初始化时去重并订阅 tick；遍历顺序不保证。调度时跳过不在交易时段的标的，逐个计算与 CTA 相同的 DualThrust 上下轨。它是代码池上的独立突破判断，不是因子排名、择优选股或组合权重优化。

| 项目 | SEL 行为 |
| --- | --- |
| 突破价格 | 用最新 K 线最高价判断上轨突破、最低价判断下轨突破 |
| 空仓信号 | 上破设目标仓位 `+trdUnit`；否则下破且非股票模式时设为 `-trdUnit` |
| 持仓退出信号 | 多仓下破或非股票空仓上破时设目标仓位为 0 |
| 股票模式 | `trdUnit=100`，禁止开空；请求 K 线时给代码追加 `-`，使用前复权代码 |
| tick/bar 回调 | 示例实现为空，不在这里产生交易信号 |

**持仓语义不能按 CTA 理解：** [SelStraBaseCtx.cpp](src/strategy_engine/sel/SelStraBaseCtx.cpp) 的调度结束处理会给没有待处理信号的已有持仓追加 `autoexit` 清仓信号；回测 [SelMocker.cpp](tests/backtest/WtBtCore/SelMocker.cpp) 也有此逻辑。当前示例在无反向突破时不主动提交维持仓位信号，因此不能描述为一直持有到反向突破。

## 6. 参数与加载方式

### 示例参数

两种示例均在 `init(WTSVariant*)` 中直接读取 `params`，没有有效的默认交易配置。下表“缺失读取值”仅指配置对象存在但字段缺失时的读取行为，不是推荐参数。

| 参数 | CTA | SEL | 含义 | 缺失读取值 |
| --- | --- | --- | --- | --- |
| `days` | 是 | 是 | 历史 K 线窗口起始索引，见窗口说明 | `0` |
| `k1` / `k2` | 是 | 是 | 上轨/下轨幅度系数 | `0.0` |
| `period` | 是 | 是 | K 线周期，须与行情数据源支持的周期一致 | 空字符串 |
| `count` | 是 | 是 | 请求的 K 线数量 | `0` |
| `code` | 是 | 否 | 单个标准交易代码 | 空字符串 |
| `codes` | 否 | 是 | 逗号分隔的标准交易代码池 | 空字符串 |
| `stock` | 是 | 是 | 股票数量单位及禁止开空开关 | `false` |

必须提供非空 `params`、有效代码和周期、足够的历史数据；不应使用上述缺失值启动策略。当前示例未校验 `days/count` 的合理性和实际返回数据长度。股票/ETF 使用时需明确配置 `stock=true`，数量、资金约束、交易时段及交易规则仍需在执行层验证；两个示例都没有算法内资金配比或独立止损参数。

### 实盘入口

当前入口为 [src/main.cpp](src/main.cpp)，由 [QtRunner.cpp](src/trade_execution/WtRunner/QtRunner.cpp) 加载配置。默认主配置名为 `config.yaml`，日志配置名为 `logcfg.yaml`，由进程工作目录解析，也可用 `-c`、`-l` 指定路径。

| 项目 | CTA | HFT |
| --- | --- | --- |
| `env.name` | `cta`，空名称也选 CTA | 使用 `hft`；当前其他非空且非 `sel` 的名称也会落入 HFT 分支 |
| 策略数组 | `strategies.cta` | `strategies.hft` |
| 工作目录下的工厂扫描目录 | `cta/` | `hft/` |
| 策略项 | `active`、`id`、`name`、`slippage`、`params` | 同左，另有 `agent`、`trader` |
| `name` 格式 | `WtCtaStraFact.DualThrust` | 外部工厂名与策略名用 `.` 连接 |

`active` 缺失时为 `false`；只有启用的项才创建策略。`env.name=sel` 虽选择 SEL 引擎，但不能直接照抄 CTA 配置宣称 SEL 实盘已可用，见第 7 节。

工厂产物在当前 Debug 构建中为 `build/build_x64/Debug/bin/libWtCtaStraFact.so` 和 `libWtSelStraFact.so`。源码移动不自动部署策略插件到运行工作目录；需按加载器规则准备插件、行情和交易模块、基础数据、数据管理、交易动作策略、执行器及路由。仓库 [config](config) 目前为空，没有完整可直接运行的配置；指定 `-c` 不会自动切换工作目录。

### 回测入口

[WtBtRunner.cpp](tests/backtest/WtBtRunner/WtBtRunner.cpp) 默认读取 `configbt.yaml`，`env.mocker` 支持 `cta/hft/sel/exec/uft`。CTA/SEL 对应配置块中的 `module` 指定工厂动态库，`strategy` 提供 `name/id/params`；这里算法名分别是 `DualThrust`、`DualThrustSelection`，不带工厂名前缀。

SEL 回测还需要 `task.date/time/period`；调度周期 `d/w/m/y/min` 与策略参数中的 K 线 `period` 是不同概念。当前无配套历史行情和完整回测配置，本说明不代表已经完成收益或交易行为验证。

## 7. 已有边界与限制

以下是本次源码核查发现的既有行为，目录整理没有修复或改变它们：

1. **SEL 实盘启动链不完整。** `QtRunner::config()` 的非 HFT 分支仍调用 CTA 策略初始化，并把执行器和路由绑定到 CTA 引擎。Porter 的 [WtRtRunner.cpp](src/trade_execution/WtPorter/WtRtRunner.cpp) 也有对应问题，其 `initSelStrategies()` 读取的仍是 `strategies.cta`。
2. **SEL 实盘工厂符号不匹配。** [SelStrategyMgr.cpp](src/strategy_engine/sel/SelStrategyMgr.cpp) 查找 `createMfStrategyFact/deleteMfStrategyFact`，示例实际导出 `createSelStrategyFact/deleteSelStrategyFact`；SEL 回测加载器使用后者。工厂能编译不等于实盘管理器能加载。
3. **SEL 实盘任务关联存在问题。** [WtSelEngine.cpp](src/strategy_engine/sel/WtSelEngine.cpp) 给任务生成独立 ID，调度却用它查找上下文；任务与上下文起始 ID 不同，不能仅凭引擎存在就宣称调度可用。
4. **示例错误处理有限。** SEL 直接解引用会话信息；某个标的 K 线为空时会结束整轮处理，而不是只跳过该标的。CTA 实盘初始化未检查策略创建结果便使用，也未处理策略 `init()` 返回失败。
5. **测试范围有限。** 现有单元测试覆盖事件通知与 XTP 模块加载，不覆盖上述策略的信号、调度或回测收益。目录重构的构建和回归测试不能替代策略行为验证，也不代表已连接柜台或具备实盘运行条件。