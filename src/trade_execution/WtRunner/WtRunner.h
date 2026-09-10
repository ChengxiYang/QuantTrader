/*!
 * /file WtRunner.h
 * /project	WonderTrader
 *
 * /author Wesley
 * /date 2020/03/30
 * 
 * /brief 
 */
#pragma once
#include <string>
#include <unordered_map>

#include "../Includes/ILogHandler.h"

#include "../../strategy_engine/WtCore/EventNotifier.h"
#include "../../strategy_engine/WtCore/CtaStrategyMgr.h"
#include "../../strategy_engine/WtCore/HftStrategyMgr.h"
#include "../../strategy_engine/WtCore/SelStrategyMgr.h"

#include "../../strategy_engine/WtCore/WtCtaEngine.h"
#include "../../strategy_engine/WtCore/WtHftEngine.h"
#include "../../strategy_engine/WtCore/WtSelEngine.h"
#include "../../strategy_engine/WtCore/WtLocalExecuter.h"
#include "../../strategy_engine/WtCore/WtDistExecuter.h"
#include "../../strategy_engine/WtCore/TraderAdapter.h"
#include "../../strategy_engine/WtCore/ParserAdapter.h"
#include "../../strategy_engine/WtCore/WtDtMgr.h"
#include "../../strategy_engine/WtCore/ActionPolicyMgr.h"

#include "../../WTSTools/WTSHotMgr.h"
#include "../../WTSTools/WTSBaseDataMgr.h"

NS_WTP_BEGIN
class WTSVariant;
class WtDataStorage;
NS_WTP_END

USING_NS_WTP;

class WtRunner : public ILogHandler
{
public:
	WtRunner();
	~WtRunner();

public:
	void init(const std::string& filename);

	bool config(const std::string& filename);

	void run(bool bAsync = false);

private:
	bool initTraders(WTSVariant* cfgTrader);
	bool initParsers(WTSVariant* cfgParser);
	bool initExecuters(WTSVariant* cfgExecuter);
	bool initDataMgr();
	bool initEvtNotifier();
	bool initCtaStrategies();
	bool initHftStrategies();
	bool initActionPolicy();

	bool initEngine();

//////////////////////////////////////////////////////////////////////////
//ILogHandler
public:
	virtual void handleLogAppend(WTSLogLevel ll, const char* msg) override;

private:
	WTSVariant*			_config;
	TraderAdapterMgr	_traders;
	ParserAdapterMgr	_parsers;
	WtExecuterFactory	_exe_factory;

	WtCtaEngine			_cta_engine;
	WtHftEngine			_hft_engine;
	WtSelEngine			_sel_engine;
	WtEngine*			_engine;

	WtDataStorage*		_data_store;

	WtDtMgr				_data_mgr;

	WTSBaseDataMgr		_bd_mgr;
	WTSHotMgr			_hot_mgr;
	EventNotifier		_notifier;

	CtaStrategyMgr		_cta_stra_mgr;
	HftStrategyMgr		_hft_stra_mgr;
	SelStrategyMgr		_sel_stra_mgr;
	ActionPolicyMgr		_act_policy;

	bool				_is_hft;
	bool				_is_sel;

	bool				_to_exit;
};

