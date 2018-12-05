#include "mfastHandler.h"

int Fix_Analyse::getData(const SSE_MarketData_Templates::MDSnapshotFullRefresh_4001_cref& sse_cref, CQdFtdcDepthMarketDataField& data, const char *protoTime)
{
		memset(&data,0x0,sizeof(CQdFtdcDepthMarketDataField));

		char mdflagarr[16] = { 0 };
		mfast::ascii_string_cref& mdflag = sse_cref.get_MDStreamID();
		if (mdflag.present())
		{
			snprintf(mdflagarr,9,"%s",mdflag.c_str());
		}
		
		mfast::ascii_string_cref& SecurityID = sse_cref.get_SecurityID();
		if (SecurityID.present())
		{
			if (strcmp(mdflagarr, "MD001") == 0)
			{
				snprintf(data.InstrumentID, 30, "%s.SSE", SecurityID.c_str());
			}
			else
			{
				snprintf(data.InstrumentID, 30, "%s", SecurityID.c_str());
			}
		}
		else
		{
			return 0;
		}

		mfast::uint64_cref& TradeVolume = sse_cref.get_TradeVolume();
		if (TradeVolume.present()) {
			data.Volume = TradeVolume.value();
		}

		mfast::decimal_cref& TotalValue = sse_cref.get_TotalValueTraded();
		if (TotalValue.present()) {
			data.Turnover = TotalValue.operator double();
		}

		mfast::decimal_cref& PrevClosePx = sse_cref.get_PrevClosePx();
		if (PrevClosePx.present()) {
			data.PreClosePrice = PrevClosePx.operator double();
		}

		mfast::decimal_cref& PrevSetPx = sse_cref.get_PrevSetPx();
		if (PrevSetPx.present()) {
			data.PreSettlementPrice = PrevSetPx.operator double();
		}

		const SSE_MarketData_Templates::MDSnapshotFullRefresh_4001_cref::MDFullGrp_cref& MDGrp = sse_cref.get_MDFullGrp();
		double tmpPrice = 0.0;
		int pos = 0;
		int tmpVolume = 0;
		if (MDGrp.present())
		{
			for (auto itr = MDGrp.begin(); itr != MDGrp.end(); ++itr)
			{
				mfast::decimal_cref& price = itr->get_MDEntryPx();
				if (price.present()) {
					tmpPrice = price.operator double();
				}
				else
					tmpPrice = 0.0;

				mfast::ascii_string_cref& MDEntryType = itr->get_MDEntryType();
				if (MDEntryType.present()) {
					const char *type = MDEntryType.c_str();
					if (type != NULL)
					{
						switch (type[0])
						{
						case '0': //买入 
						{

							mfast::uint64_cref& MDEntrySize = itr->get_MDEntrySize();
							if (MDEntrySize.present()) {
								tmpVolume = MDEntrySize.value();
							}

							pos = 5;
							mfast::uint32_cref& MDEntryPositionNo = itr->get_MDEntryPositionNo();
							if (MDEntryPositionNo.present()) {
								pos = MDEntryPositionNo.value();
								//printf("case 0: pos=%d\tprice=%f size=%ld\n",pos,tmpPrice,tmpVolume);
							}
							switch (pos)
							{
							case 0:
								data.BidPrice1 = tmpPrice;
								data.BidVolume1 = tmpVolume;
								break;
							case 1:
								data.BidPrice2 = tmpPrice;
								data.BidVolume2 = tmpVolume;
								break;
							case 2:
								data.BidPrice3 = tmpPrice;
								data.BidVolume3 = tmpVolume;
								break;
							case 3:
								data.BidPrice4 = tmpPrice;
								data.BidVolume4 = tmpVolume;
								break;
							case 4:
								data.BidPrice5 = tmpPrice;
								data.BidVolume5 = tmpVolume;
								break;
							default:
								break;
							}
							break;
						}
						case '1': //卖出
						{
							mfast::uint64_cref& MDEntrySize = itr->get_MDEntrySize();
							if (MDEntrySize.present()) {
								tmpVolume = MDEntrySize.value();
							}

							pos = 5;
							mfast::uint32_cref& MDEntryPositionNo = itr->get_MDEntryPositionNo();
							if (MDEntryPositionNo.present()) {
								pos = MDEntryPositionNo.value();
								//printf("case 0: pos=%d\tprice=%f size=%ld\n", pos, tmpPrice, tmpVolume);
							}
							switch (pos)
							{
							case 0:
								data.AskPrice1 = tmpPrice;
								data.AskVolume1 = tmpVolume;
								break;
							case 1:
								data.AskPrice2 = tmpPrice;
								data.AskVolume2 = tmpVolume;
								break;
							case 2:
								data.AskPrice3 = tmpPrice;
								data.AskVolume3 = tmpVolume;
								break;
							case 3:
								data.AskPrice4 = tmpPrice;
								data.AskVolume4 = tmpVolume;
								break;
							case 4:
								data.AskPrice5 = tmpPrice;
								data.AskVolume5 = tmpVolume;
								break;
							default:
								break;
							}
							break;
						}
						case '2': //最新价, 以及行情最后修改时间 
						{
							data.LastPrice = tmpPrice;
							//printf("case 2\n");
							mfast::ascii_string_cref& YMD = itr->get_MDEntryTime();
							if (YMD.present()) 
							{
								string tmptime = YMD.data();
								//printf("tmptime.length()=%d\n", sizeof(tmptime.c_str()));
								if (tmptime.length() >= 8) {
									string HMS = tmptime.substr(0, 2) + ':' + tmptime.substr(2, 2) + ':' + tmptime.substr(4, 2);
									memcpy(data.UpdateTime, HMS.c_str(), 8);
									data.UpdateMillisec = 10 * atoi(tmptime.substr(6, 2).c_str());//交易所传过来的毫秒只有两位
									//printf("%s\n",data.UpdateTime);
								}
							}

							break;
						}
						case '4': //开盘
							data.OpenPrice = tmpPrice;
							//printf("case 4\n");
							break;
						case '5': //收盘
							data.ClosePrice = tmpPrice;
							//printf("case 5\n");
							break;
						case '6': //今结算
							data.SettlementPrice = tmpPrice;
							//printf("case 6\n");
							break;
						case '7': //当日最高价
							data.HighestPrice = tmpPrice;
							//printf("case 7\n");
							break;
						case '8': //当日最低价
							data.LowestPrice = tmpPrice;
							// printf("case 8\n");
							break;
						case 'x': //动态参考
								  //printf("case x\n");
							break;
						default:
							break;
						}
					}
					else { continue; }
				}

			}
		}
		else { return 0; }
		mfast::ascii_string_cref& tradeStatus = sse_cref.get_TradingPhaseCode();
		if (tradeStatus.present())
		{
			const char * status = tradeStatus.c_str();
			if (status != NULL) {
				//if (status[0] == 'E')
					//return -1;
				if (status[0] == 'P')
					data.LastPrice = DBL_MAX;
				data.InstrumentStatus = status[0];
			}
			
			//printf("tradeStatus=%c\n", data.InstrumentStatus);
		}
		memcpy(data.TradingDay, tradingday, 8);
		memcpy(data.ExchangeID,"SSE",3);
		if (strncmp(mdflagarr, "MD001",5) == 0)
		{
			memcpy(data.UpdateTime,protoTime,8);
			data.UpdateMillisec = atoi(protoTime+9);
		}
	
	return 1;
}

inline int Fix_Analyse::fix_analyse(const char * pbuffer, int nSize, const char * upflag, const char *protoTime)
{
	CQdFtdcDepthMarketDataField data;
	memset(&data, 0x0, sizeof(CQdFtdcDepthMarketDataField));
	const char* first = pbuffer;
	const char* last = pbuffer + nSize;
	//decoder->
	try {
		mfast::fast_decoder decoder;
		const mfast::templates_description* description[] = { SSE_MarketData_Templates::description() };
		decoder.include(description);

		while (first < last)
		{
			mfast::message_cref& msg_ref = decoder.decode(first, last);
			const SSE_MarketData_Templates::MDSnapshotFullRefresh_4001_cref& sse_cref = msg_ref;

			//if decode error, it won't send out the data
			if (getData(sse_cref, data,protoTime) == 1)
			{
				//process your data
			}
		}
	}
	catch (std::exception& e) 
	{
		REPORT_EVENT(LOG_ERROR, "Fix", "catch analyse::exception=%s", e.what());
		return 0;
	}
	
	return 0;
}

void Fix_Analyse::setTradingDay()
{
#ifdef WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);//st.wYear, st.wMonth ,st.wDay
	memset(tradingday,0x0,9);
	sprintf(tradingday,"%d%02d%02d",st.wYear,st.wMonth,st.wDay);
#else
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
#endif
}

Fix_Analyse* Fix_Analyse::m_instance = new Fix_Analyse;

Fix_Analyse* Fix_Analyse::getinstance()
{
    return m_instance;
}


