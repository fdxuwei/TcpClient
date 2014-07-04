#pragma once

#include "StdAfx.h"
#include <sstream>
#include <string>
#include <map>


// 仅支持c++标准类型，以及string
#define PROPERTY(type,name)\
	type Get##name(){return GetProperty<type>(#name);  }\
	void Set##name(type _##name){ if(Get##name() != _##name)SetProperty(#name, _##name);}

typedef struct 
{
	CString csValue;
	BOOL bChanged;
}PropertyValueType;



class CConfig
{
public:
	static CConfig& CConfig::Instance()
	{
		static CConfig cc;
		return cc;
	}

	~CConfig(void);
	//
	BOOL Load();
	BOOL Save();

	// 属性定义
	PROPERTY(CString, ServIP)
	PROPERTY(unsigned, ServPort)
	PROPERTY(int, ClientNum)
	PROPERTY(int, ThreadNum)
	PROPERTY(int, MsgSize)
	PROPERTY(int, SendCycle)

private:
	CConfig(void);

	// sqlite
	std::map<CString, PropertyValueType> m_mapProperty;

	template<class T>
	T GetProperty(LPCTSTR lpName);

	template<class T>
	void SetProperty(LPCTSTR lpName, T value);
};

template<class T>
inline T CConfig::GetProperty(LPCTSTR lpName)
{
	CString csValue("");
	if(m_mapProperty.find(CString(lpName)) != m_mapProperty.end())
	{
		csValue = m_mapProperty[CString(lpName)].csValue;
	}
	std::stringstream ss(csValue.GetBuffer(0));
	T ret;
	ss >> ret;
	return ret;
}

template<class T>
inline void CConfig::SetProperty(LPCTSTR lpName, T value)
{
	PropertyValueType pvt;
	std::stringstream ssValue;
	ssValue << value;
	pvt.csValue = CString(ssValue.str().c_str());
	pvt.bChanged = TRUE;
	m_mapProperty[CString(lpName)] = pvt;
}

std::ostream& operator<<(std::stringstream& ss, const CString& csValue);

std::istream& operator>>(std::stringstream& ss, CString& csValue);
