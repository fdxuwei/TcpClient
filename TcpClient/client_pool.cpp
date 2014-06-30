#include "StdAfx.h"
#include "client_pool.h"

using namespace std;

client_pool::client_pool(int clinum, const std::string &servip, unsigned servport, int threadnum, int msgsize)
	: m_isp(threadnum)
{
	for(int i = 0; i < clinum; ++i)
	{
		m_clients.push_back(tcp_client_ptr(new tcp_client(m_isp.get_io_service(), servip, servport, msgsize)));
	}
}

void client_pool::start()
{
	int success_count = 0;
	for(std::list<tcp_client_ptr>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		(*it)->connect();
	}
	m_threadp.reset(new boost::thread(boost::bind(&io_service_pool::run, &m_isp)));
}

void client_pool::stop()
{
	m_isp.stop();
}

client_pool::~client_pool(void)
{
	m_clients.clear();
}


int client_pool::get_connoknum()
{
	int ret = 0;
	for(std::list<tcp_client_ptr>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		if((*it)->is_connected())
			++ret;
	}
	return ret;
}

int client_pool::get_errnum()
{
	int ret = 0;
	for(std::list<tcp_client_ptr>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		ret += (*it)->get_errcount();
	}
	return ret;
}

int client_pool::get_writebytes()
{
	int ret = 0;
	for(std::list<tcp_client_ptr>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		ret += (*it)->get_writebytes();
	}
	return ret;
}

int client_pool::get_readbytes()
{
	int ret = 0;
	for(std::list<tcp_client_ptr>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		ret += (*it)->get_readbytes();
	}
	return ret;
}

int client_pool::get_conndelay()
{
	int ret = 0;
	for(std::list<tcp_client_ptr>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		ret += (*it)->get_conndelay();
	}
	return ret/m_clients.size();
}

int client_pool::get_sendrecvdelay()
{
	int ret = 0;
	for(std::list<tcp_client_ptr>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		ret += (*it)->get_rdwrdelay();
	}
	return ret/m_clients.size();
}