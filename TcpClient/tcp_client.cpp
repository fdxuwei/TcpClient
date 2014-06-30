#include "StdAfx.h"
#include "tcp_client.h"
#include <boost/bind.hpp>
#include <boost/shared_array.hpp>

using namespace boost::asio;

tcp_client::tcp_client(io_service &ios, const std::string &servip, unsigned servport, int msgsize)
	: m_ios (ios)
	, m_sock (ios)
	, m_servip (servip)
	, m_servport (servport)
	, m_msgsize (msgsize)
{
}

void tcp_client::connect()
{
	m_sock.async_connect(ip::tcp::endpoint(ip::address_v4::from_string(m_servip), m_servport),
		boost::bind(&tcp_client::connect_handler, this, _1));
	m_starttime = boost::chrono::system_clock::now();
}

void tcp_client::connect_handler(const boost::system::error_code &ec)
{
	boost::shared_array<char> sabuf(new char[m_msgsize]);

	if(ec)
	{
		++m_errcount;
		m_connected = false;
	}
	else
	{
		m_connected = true;
		//
		boost::chrono::milliseconds dur = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now() - m_starttime);
		m_conndelay = dur.count();
		//
		m_starttime = boost::chrono::system_clock::now();
		m_sock.async_write_some(mutable_buffers_1(sabuf.get(), m_msgsize),
			boost::bind(&tcp_client::read_handler, this, _1, _2));
	}
}

void tcp_client::write_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
	if(ec)
	{
		++m_errcount;
	}
	else
	{
		m_writebytes += bytes_transferred;
		//
		//
		m_sock.async_read_some(mutable_buffers_1(m_buf, sizeof(m_buf)),
			boost::bind(&tcp_client::read_handler, this, _1, _2));
	}
}


void tcp_client::read_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
	boost::shared_array<char> sabuf(new char[m_msgsize]);
	if(ec)
	{
		++m_errcount;
	}
	else
	{
		m_readbytes += bytes_transferred;
		//
		boost::chrono::milliseconds dur = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now() - m_starttime);
		m_rdwrdelay = dur.count();
		//
		m_starttime = boost::chrono::system_clock::now();
		m_sock.async_write_some(const_buffers_1(sabuf.get(), m_msgsize),
			boost::bind(&tcp_client::write_handler, this, _1, _2));
	}
}

void tcp_client::close()
{
	m_sock.close();
}

tcp_client::~tcp_client(void)
{
	close();
}
