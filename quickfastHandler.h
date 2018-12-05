#include <string>
#include <fstream>
#include <Common/QuickFASTPch.h>
#include <Codecs/XMLTemplateParser.h>
#include <Codecs/TemplateRegistry.h>
#include <Codecs/Decoder.h>
#include <Codecs/DataSourceBuffer.h>
#include <Codecs/SingleMessageConsumer.h>
#include <Codecs/GenericMessageBuilder.h>
#include <Messages/Sequence.h>
#include <fstream>

using namespace std;
using namespace QuickFAST;

class Fix_Analyse
{
public:
	
	~Fix_Analyse(){
		if (m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL ;
		}
		delete decoder;
		decoder = NULL;
		delete builder;
		builder = NULL;
	};
	//从数据中解析出结构体
	int GetData(const Messages::Message& msg, CQdFtdcDepthMarketDataField & outData, const char *tradingDay,const char *protoTime);
	//初始化解析配置
	int prepare();
	//数据入口
	int fix_analyse(const unsigned char * pbuffer, int nSize, const char * upflag,const char *protoTime);
	//设置模板的路径
	int setTemplateFile(string &path);

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
	//模板路径
	string templateFilePATH;
	//解析模板类
	Codecs::TemplateRegistryPtr templateRegistry;
	//
	Codecs::Decoder *decoder;
	//
	Codecs::GenericMessageBuilder *builder;
	//返回decode message类
	Codecs::SingleMessageConsumer consumer;

private:
    static Fix_Analyse * m_instance;

	char tradingday[16];
};

