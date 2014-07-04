#pragma once

class CHexStr
{
public:
	CHexStr(void);
	~CHexStr(void);
	static CString BufferToStr(const void *pData, int nSize);
	static void StrToBuffer(const CString &csHex, void *pData, int &nSize);
	static BOOL IsValidHexStr(const CString &csHex);
	static BOOL IsAscii(const void *pData, int nSize);
	static int CalBufferSize(const CString &csHex);

private:
	static char HexToDec(char chHex);
};
