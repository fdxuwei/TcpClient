#include "StdAfx.h"
#include "HexStr.h"

CHexStr::CHexStr(void)
{
}

CHexStr::~CHexStr(void)
{
}

CString CHexStr::BufferToStr(const void *pData, int nSize)
{
	CString csHex;
	CString csChar;
	for(int i = 0; i < nSize; ++i)
	{
		csChar.Format(_T("%02x"), *((unsigned char*)(pData)+i));
		csHex += csChar + _T(" ");
	}
	return csHex;
}

void CHexStr::StrToBuffer(const CString &csHex, void *pData, int &nSize)
{
	unsigned char *pch = (unsigned char*)pData;
	CString csStr = csHex;
	csStr.Remove(' ');// É¾³ý¿Õ¸ñ
	if(csStr.GetLength()%2 != 0) // invalid hex string
	{
		nSize = 0;
		return;
	}
	for(int i = 0; i < csStr.GetLength(); i += 2)
	{
		*pch = HexToDec(csStr.GetAt(i))*16 + HexToDec(csStr.GetAt(i+1));
		++pch;
	}
	nSize = csStr.GetLength() / 2;
}

BOOL CHexStr::IsValidHexStr(const CString &csHex)
{
	CString csStr = csHex;
	csStr.Remove(' ');// É¾³ý¿Õ¸ñ
	if(csStr.GetLength()%2 != 0)
		return FALSE;
	csStr.MakeUpper();
	for(int i = 0; i < csStr.GetLength(); ++i)
	{
		char ch = csStr.GetAt(i);
		if(ch < '0' 
			|| (ch > '9' && ch < 'A')
			|| (ch > 'F'))
			return FALSE;
	}
	return TRUE;
}

char CHexStr::HexToDec(char chHex)
{
	chHex = toupper(chHex);
	return ((chHex >= '0' && chHex <= '9') ? (chHex-'0') : (chHex-'A'+10));
}

int CHexStr::CalBufferSize(const CString &csHex)
{
	CString csStr = csHex;
	csStr.Remove(' ');
	if(csStr.GetLength()%2 != 0)
		return 0;
	return csStr.GetLength()/2;
}

BOOL CHexStr::IsAscii(const void *pData, int nSize)
{
	for(int i = 0; i < nSize; ++i)
	{
		if(!isprint(*((unsigned char*)pData) + i))
			return FALSE;
	}
	return TRUE;
}