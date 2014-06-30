#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <string>

#include "tcp_statics.h"

class tcp_client : public tcp_statics
{
public:
	tcp_client(boost::asio::io_service &ios, const std::string &servip, unsigned servport, int msgsize);
	~tcp_client(void);

	void connect();
	void close();

private:
	void connect_handler(const boost::system::error_code &ec);
	void read_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);
	void write_handler(const boost::system::error_code &ec, std::size_t bytes_transferred);
	std::string m_servip;
	unsigned m_servport;
	int m_msgsize;
	boost::asio::io_service &m_ios;
	boost::asio::ip::tcp::socket m_sock;
	boost::chrono::system_clock::time_point m_starttime;
	char m_buf[1024];
};

typedef boost::shared_ptr<tcp_client> tcp_client_ptr;
