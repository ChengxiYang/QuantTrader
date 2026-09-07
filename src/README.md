# wt > src
这是wondertrader的C++底层源码

## 开发环境
+ Windows	
	> `Visual Studio 2017` + `Windows 10`
+ Linux	
	> `Gcc v8.4.0` + `cmake 3.17.5`

## 依赖库
+ [boost 1.72](https://www.boost.org/)
+ [rapidjson 1.0.2](https://github.com/Tencent/rapidjson)
+ [spdlog 1.9.2](https://github.com/gabime/spdlog)
+ [nanomsg 1.1.5](https://github.com/nanomsg/nanomsg)

## 解决方案结构
+ ***基础库***
	- Includes、Share、WTSUtils、WTSTools：公共数据结构、接口、工具、日志和配置能力
+ ***行情与数据***
	- ParserXeleSkt：艾科朗科组播行情解析器
	- WtDtCore、WtDtPorter、WtDtHelper、WtDtServo：行情接入和数据服务核心
	- WtDataStorage、WtDataStorageAD：行情数据读写
+ ***交易***
	- TraderATP：华锐 ATP 股票交易柜台适配器
	- TraderXTP：XTP 股票交易柜台适配器
	- WtCore、WtPorter、WtRunner：实盘交易核心及运行入口
+ ***策略与回测***
	- WtCtaStraFact、WtSelStraFact：CTA 和 SEL 策略工厂
	- WtBtCore、WtBtPorter、WtBtRunner：回测核心及运行入口