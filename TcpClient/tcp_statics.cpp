#include "StdAfx.h"
#include "tcp_statics.h"

tcp_statics::tcp_statics(void)
	: m_errcount (0)
	, m_readbytes (0)
	, m_writebytes (0)
	, m_connected (false)
	, m_conndelay (0)
	, m_rdwrdelay (0)
{
}

tcp_statics::~tcp_statics(void)
{
}
