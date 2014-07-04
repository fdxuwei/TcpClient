#pragma once
#include "tcp_client.h"
#include "io_service_pool.h"
#include <list>
#include <string>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

class client_pool
{
public:
	client_pool(int clinum, const std::string &servip, unsigned servport, int threadnum);
	~client_pool(void);
	void start(); 
	void stop();
	void send(const void *data, size_t size);
	// statics
	int get_connoknum();
	int get_errnum();
	int get_writebytes();
	int get_readbytes();
	int get_conndelay();
	int get_sendrecvdelay();

private:
	io_service_pool m_isp;
	std::list<tcp_client_ptr> m_clients;
	boost::shared_ptr<boost::thread> m_threadp;
};
