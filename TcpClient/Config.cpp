#include "StdAfx.h"
#include "Config.h"
#include "Markup.h"

using namespace std;

#define CONFIG_FILE_NAME "config.xml"

CConfig::CConfig(void)
{
}

CConfig::~CConfig(void)
{
}

BOOL CConfig::Load()
{
	// 清理数据
	CMarkup xml;
	xml.Load(CONFIG_FILE_NAME);
	CString csKey, csValue;
	while(xml.FindElem())
	{
		csKey = xml.GetTagName();
		csValue = xml.GetData();
		SetProperty(csKey, csValue);
	}
	return TRUE;
}

BOOL CConfig::Save()
{
	CMarkup xml;
	// 把属性值存到磁盘
	for(map<CString,PropertyValueType>::iterator it = m_mapProperty.begin(); it != m_mapProperty.end(); ++it)
	{
		PropertyValueType *ppv = &(it->second);
		if(ppv->bChanged)
		{
			const CString* pcsName = &(it->first);
			const CString* pcsValue = &(ppv->csValue);
			xml.AddElem(*pcsName, *pcsValue);
		}
	}
	xml.Save(CONFIG_FILE_NAME);
	return TRUE;
}


std::ostream& operator<<(std::stringstream& ss, const CString& csValue)
{
	std::string sTemp = (LPCTSTR)csValue;
	return (ss << sTemp);
}

std::istream& operator>>(std::stringstream& ss, CString& csValue)
{
	std::string sTemp;
	ss >> sTemp;
	csValue = sTemp.c_str();
	return ss;
}