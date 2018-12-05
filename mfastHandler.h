#include <string>
#include <fstream>

#include "SSE_MarketData_Templates.h"
#include <mfast.h>
#include <mfast/coder/fast_decoder.h>
#include <mfast/json/json.h>
#include <mfast/xml_parser/dynamic_templates_description.h>

using std::string;
using std::ostringstream;
using std::cout;
using std::endl;

using mfast::templates_description;
using mfast::dynamic_templates_description;
using mfast::fast_decoder;
using mfast::message_cref;
using mfast::ascii_string_cref;
using mfast::json::encode;

//using namespace std;

class Fix_Analyse
{
public:
	
	~Fix_Analyse(){
		if (m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL ;
		}
	};

	int getData(const SSE_MarketData_Templates::MDSnapshotFullRefresh_4001_cref& sse_cref, CQdFtdcDepthMarketDataField& data, const char *protoTime);
	//数据入口
	int fix_analyse(const char * pbuffer, int nSize, const char * upflag, const char *protoTime);

	void setTradingDay();

	static Fix_Analyse* getinstance();
	static void DestoryInstance()
	{
		if (m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL ;
		}
	}

private:
	Fix_Analyse(){};
	Fix_Analyse(const Fix_Analyse &){};
	Fix_Analyse& operator=(const Fix_Analyse&){}

private:
    static Fix_Analyse * m_instance;
	char tradingday[16];
};

