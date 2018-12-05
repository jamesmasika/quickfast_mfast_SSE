#include "quickfastHandler.h"

#define SET_BIDPRICE_MARKETDATA(name)								\
				outData.BidPrice##name = dec.operator double();		\

#define SET_BIDVOLUME_MARKETDATA(name)								\
				outData.BidVolume##name = value->toUInt64();		\

#define SET_ASKPRICE_MARKETDATA(name)								\
				outData.AskPrice##name = dec.operator double();		\

#define SET_ASKVOLUME_MARKETDATA(name)								\
				outData.AskVolume##name = value->toUInt64();		\


//vector<string> strsplit2(const string& str, string separate_character)
//{
//	vector<string> vestr;
//	int separate_characterLen = separate_character.size();
//	int lastPosition = 0;
//	int index = -1;
//	while (-1 != (index = str.find(separate_character,lastPosition)))
//	{
//		vestr.push_back(str.substr(lastPosition,index - lastPosition));
//		lastPosition = index + separate_characterLen;
//	}
//	string lastString = str.substr(lastPosition);
//	if (!lastString.empty())
//	{
//		vestr.push_back(lastString);
//	}
//	return vestr;
//}

extern int a = 0;
//LARGE_INTEGER t1,t2,tc;
void ip_frag_Fast(char * skb)
{
	char *tskb = skb;
	tskb+=ipandudpLen;
	tskb+=13;					//8=FIXT.1.1.9=  

	//LARGE_INTEGER t1,t2,tc;
    //QueryPerformanceFrequency(&tc);
    //QueryPerformanceCounter(&t1);
	//printf("start analyse\n");
	/*******************************************************************************************************/
	//const char *p35w = "35=W";		//这里代表是否有fast压缩数据 没有则直接返回
	//const char *P167 = "167=01";	// 这里代表行情类型 01-股票 02-衍生品 
	//const char *p75day = "75=";		//这里取出交易日
	//const char *p265up = "265=";	//这里取出全量标志位
	//const char *p95l = "95=";		// 这里取出 fast数据的长度
	//const char *p96rawdata = "96="; // 这里将指针指向fast数据开始
	char p35w[5] = "35=W";
	char protoTime52[4] = "52=";
	char P167[7] = "167=01";
	char p75day[4] = "75=";
	char p265up[5] = "265=";
	char p95l[4] = "95=";
	char p96rawdata[4] = "96=";
	char *p;
	p = strstr(tskb,p35w);
	if(p==NULL)//找不到该串
	{
		//printf("35 miss\n");
		return ;
	}

	char exptime[13] = {0};
	p = strstr(tskb,"52=");
	if(p == NULL)
	{
		//printf("52 miss\n");
		return;
	}
	tskb = p + 12;
	memcpy(exptime,tskb,12);
	//printf("time=%s\n",exptime);

	if(RecOpt == 1) 
	{}
	else
	{
		p = strstr(tskb,P167);
		if(p==NULL)
		{
			//printf("P167 miss\n");
			return ;
		}
		tskb = p;
	}
	
	

	
	//p = strstr(tskb,"779=");
	//char tradingDay[9]={0};
	//memcpy(tradingDay,p+3,8);
	//tskb = p+3;

	char upflag[4]={0};
	//p = strstr(tskb,p265up);
	//memcpy(upflag,p+4,3);
	//tskb = p + 4;

	p = strstr(tskb,p95l);
	if(p==NULL)//找不到该串
	{
		//printf("95 miss\n");
		return ;
	}
	p = p+3;
	char rawlen[5]={0};
	char *prawlen = rawlen;
	while(*p != '\001') {
		memcpy(prawlen,p,1);
		prawlen++;
		p++;
	}
	int realrawlen = atoi(rawlen);
	tskb = p;
	p = strstr(tskb,p96rawdata);
	if(p==NULL)
	{
		//printf("96 miss\n");
		return ;
	}
	p = p+3;
	char *prealData = p;

	//printf("cap a packge\n");
	Fix_Analyse *tmpfix = Fix_Analyse::getinstance();
	tmpfix->fix_analyse((const unsigned char *)prealData,realrawlen, upflag,exptime);

	//QueryPerformanceCounter(&t2);
	//printf("Use Time:%f\n",(t2.QuadPart - t1.QuadPart)*1.0/tc.QuadPart);

	/*******************************************************************************************************/
}
/***********************************************************************************/


int Fix_Analyse::GetData(const Messages::Message& msg, CQdFtdcDepthMarketDataField & outData, const char *tradingDay,const char *protoTime)
{
#ifdef DEBUG_MORE
	REPORT_EVENT(LOG_INFO,"qmarketFix","enter decode process");
#endif
	memset(&outData,0x0,sizeof(CQdFtdcDepthMarketDataField));


		Messages::FieldCPtr value;
		char mdflag[16] = {0};
		
		if(msg.getField("MDStreamID", value))
		{
			StringBuffer buf = value->toString();
			if(!buf.empty())
			{
				snprintf(mdflag,9,"%s", (const char*)buf.data());
			}
			//printf("mdflag=%s\n",mdflag);
			//REPORT_EVENT(LOG_INFO,"qmarketFix","mdflag=%s",mdflag);
		}

		if(msg.getField("SecurityID", value))
		{
			StringBuffer buf = value->toString();
			if(!buf.empty())
			{
				if(strcmp(mdflag,"MD001") == 0)
				{
					snprintf(outData.InstrumentID, 30, "%s.SSE", (const char*)buf.data());
				}
				else 
				{
					snprintf(outData.InstrumentID, 30, "%s", (const char*)buf.data());  
				}
				//REPORT_EVENT(LOG_INFO,"qmarketFix","ID=%s",outData.InstrumentID);
			}
			else
			{ return 0; }
		}
		else
		{ return 0; }

		if(msg.getField("TradeVolume", value))
		{
			outData.Volume = (int)(value->toUInt64());
		}

		if(msg.getField("TotalValueTraded", value))
		{
			Decimal dec = value->toDecimal();
			outData.Turnover = dec.operator double();
		}

		if(msg.getField("PrevClosePx", value))
		{
			Decimal dec = value->toDecimal();
			outData.PreClosePrice = dec.operator double();
		}

		if(msg.getField("PrevSetPx", value))
		{
			Decimal dec = value->toDecimal();
			outData.PreSettlementPrice = dec.operator double();
		}

		if(msg.getField("MDFullGrp", value))
		{
			const Messages::SequenceCPtr& pSeq = value->toSequence();
			for ( int i = 0; i < pSeq->size(); i++)
			{
				const Messages::FieldSetCPtr& fieldSetPtr = pSeq->operator [](i);

				double tmpPrice = 0.0;
				int tmpVolume = 0;
				if(fieldSetPtr->getField("MDEntryPx", value))
				{
					Decimal dec = value->toDecimal();
					tmpPrice = dec.operator double();
				}

				if(fieldSetPtr->getField("MDEntryType", value))
				{
					StringBuffer buf = value->toString();
					
					if(buf.empty())
					{ continue; }

					int pos = 0;
					switch (buf[0])
					{
					case '0': //买入
						{
							pos = 6;//避免默认0时赋值给一档行情
							if(fieldSetPtr->getField("MDEntrySize", value))
							{
								tmpVolume = value->toUInt64();
							}

							if(fieldSetPtr->getField("MDEntryPositionNo", value))
							{
								pos = value->toUInt32();
							}
							switch(pos)
							{
							case 0:
								outData.BidPrice1 = tmpPrice;
								outData.BidVolume1 = tmpVolume;
								break;
							case 1:
								outData.BidPrice2 = tmpPrice;
								outData.BidVolume2 = tmpVolume;
								break;
							case 2:
								outData.BidPrice3 = tmpPrice;
								outData.BidVolume3 = tmpVolume;
								break;
							case 3:
								outData.BidPrice4 = tmpPrice;
								outData.BidVolume4 = tmpVolume;
								break;
							case 4:
								outData.BidPrice5 = tmpPrice;
								outData.BidVolume5 = tmpVolume;
								break;
							}
					
							break;
						}
					case '1': //卖出
						{
							pos = 6;//避免默认0时赋值给一档行情
							if(fieldSetPtr->getField("MDEntrySize", value))
							{
								tmpVolume = value->toUInt64();
							}

							if(fieldSetPtr->getField("MDEntryPositionNo", value))
							{
								pos = value->toUInt32();
							}
							switch(pos) 
							{
							case 0:
								outData.AskPrice1 = tmpPrice;
								outData.AskVolume1 = tmpVolume;
								break;
							case 1:
								outData.AskPrice2 = tmpPrice;
								outData.AskVolume2 = tmpVolume;
								break;
							case 2:
								outData.AskPrice3 = tmpPrice;
								outData.AskVolume3 = tmpVolume;
								break;
							case 3:
								outData.AskPrice4 = tmpPrice;
								outData.AskVolume4 = tmpVolume;
								break;
							case 4:
								outData.AskPrice5 = tmpPrice;
								outData.AskVolume5 = tmpVolume;
								break;
							}
					
							break;
						}
					case '2': //最新价, 以及行情最后修改时间
						{
							outData.LastPrice =tmpPrice;
							//行情时分秒
							if(fieldSetPtr->getField("MDEntryTime", value))
							{
								StringBuffer buf = value->toString();
								if(buf.length() >= 8)
								{
									std::string tmptime = (const char*)buf.data();
									
									#ifdef DEBUG_MORE
										REPORT_EVENT(LOG_INFO,"qmarketFix","tmptime = %s",tmptime.c_str());
									#endif
									std::string HMS = tmptime.substr(0,2) + ':' + tmptime.substr(2,2) + ':' + tmptime.substr(4,2);
									memcpy(outData.UpdateTime,HMS.c_str(),8);
									outData.UpdateMillisec = 10*atoi(tmptime.substr(6,2).c_str());//交易所传过来的毫秒只有两位=
								}
								/*else
								{return 0;}*/
							}
							/*else
							{return 0;}*/

							break;
						}
					case '4': //开盘
						outData.OpenPrice = tmpPrice;
						break;
					case '5': //收盘
						outData.ClosePrice = tmpPrice;
						break;
					case '6': //今结算
						outData.SettlementPrice = tmpPrice;
						break;
					case '7': //当日最高价
						outData.HighestPrice = tmpPrice;
						break;
					case '8': //当日最低价
						outData.LowestPrice = tmpPrice;
						break;
					case 'x': //动态参考
						break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			return 0;
		}

		if(msg.getField("TradingPhaseCode", value))
		{
			StringBuffer buf = value->toString();
			if(!buf.empty())
			{
				//REPORT_EVENT(LOG_INFO,"qmarketFix","TradingPhaseCode = %s",buf.data());
				/*if ( buf[0] == 'E')
				{
					return 0;
				}*/
				if (buf[0] == 'P')
				{
					outData.LastPrice = DBL_MAX;
				}
				outData.InstrumentStatus = buf[0];
			}
		}
		//行情交易日
		memcpy(outData.TradingDay,tradingDay,8);
		memcpy(outData.ExchangeID,"SSE",3);

		if(strcmp(mdflag,"MD001") == 0)
		{
			memcpy(outData.UpdateTime,protoTime,8);
			outData.UpdateMillisec = atoi(protoTime+9);
			//printf("md001's ID=%s time=%s-%d\n",outData.InstrumentID,outData.UpdateTime,outData.UpdateMillisec);
		}

	return 1;

}



int Fix_Analyse::prepare()
{
	// 读模板
	std::ifstream templateStream(templateFilePATH.c_str(), std::ifstream::binary);
	if(!templateStream.is_open()) {
		REPORT_EVENT(LOG_CRITICAL, "Fix", "can't open template file: %s",templateFilePATH.c_str());
		printf("解析模板找不到\n");
		return 0; 
	}
	try{
		Codecs::XMLTemplateParser parser;
		templateRegistry = parser.parse(templateStream);
		builder = new Codecs::GenericMessageBuilder(consumer);
		decoder = new Codecs::Decoder(templateRegistry);
	}catch(std::exception &e) //这里可以捕获模板文件语法异常 
	{
		REPORT_EVENT(LOG_CRITICAL, "Fix", "template file syntax error: %s",e.what());
		printf("decode template file syntax error: %s\n",e.what());
		return 0;
	}
	return 1;
}


inline int Fix_Analyse::fix_analyse(const unsigned char * pbuffer, int nSize, const char * upflag,const char *protoTime)
{
	Codecs::DataSourceBuffer source(pbuffer, nSize);
	
	CQdFtdcDepthMarketDataField data;
	try{
		//每次解析必须reset,解析字典要更新 20171218 wyl
		decoder->reset();
		while(source.messageAvailable())
		{
			decoder->decodeMessage(source, *builder);
			Messages::Message& msgOut = consumer.message();
			if( GetData(msgOut, data, tradingday, protoTime) == 1 ) 
			{
				//
			}
		}
	}
	catch (std::exception& e) 
    {  
		REPORT_EVENT(LOG_ERROR, "Fix","catch analyse::exception=%s",e.what());
        //printf("catch std::exception=%s",e.what());  
		return 0;
    }

	return 0;
}

int Fix_Analyse::setTemplateFile(string &path)
{
	this->templateFilePATH = path;
	return 1;
}

void Fix_Analyse::setTradingDay()
{
#ifdef WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);//st.wYear, st.wMonth ,st.wDay
	memset(tradingday,0x0,9);
	sprintf(tradingday,"%d%02d%02d",st.wYear,st.wMonth,st.wDay);
	//memcpy(tradingday,st.wYear,8);
	//datenow.y = st.wYear; datenow.m = st.wMonth; datenow.d = st.wDay;
#else
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	//datenow.y = p->tm_year + 1900; datenow.m = p->tm_mon + 1; datenow.d = p->tm_mday;
#endif
}

Fix_Analyse* Fix_Analyse::m_instance = new Fix_Analyse;

Fix_Analyse* Fix_Analyse::getinstance()
{
    return m_instance;
}


int main(void)
{
	Fix_Analyse *fix = Fix_Analyse::getinstance();
	string spath="SSE.xml";
	fix->setTemplateFile(spath);
	fix->setTradingDay();
	if(fix->prepare() == 1)
		ip_frag_Fast("data");
	else
		printf("未能启动监听程序\n");
	return 0;
}
