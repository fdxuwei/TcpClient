#include "StdAfx.h"
#include "tcp_client.h"
#include "CppLog.h"
#include <boost/bind.hpp>
#include <boost/shared_array.hpp>
#include "TcpClient.h"
#include "TcpClientDlg.h"

using namespace boost::asio;

tcp_client::tcp_client(io_service &ios, const std::string &servip, unsigned servport)
	: m_ios (ios)
	, m_sock (ios)
	, m_servip (servip)
	, m_servport (servport)
	, m_rctimer (ios)
	, m_reconn_time (2)
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
	if(ec)
	{
		LOG_INFO(ec.message());
		++m_errcount;
		reconnect();
	}
	else
	{
		m_connected = true;
		m_reconn_time = 2;
		//
		boost::chrono::milliseconds dur = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now() - m_starttime);
		m_conndelay = dur.count();
		//
		m_sock.async_read_some(mutable_buffers_1(m_buf, sizeof(m_buf)),
			boost::bind(&tcp_client::read_handler, this, _1, _2));
	}
}


void tcp_client::read_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
	if(ec)
	{
		LOG_INFO(ec.message());
		++m_errcount;
		reconnect();
	}
	else
	{
		m_readbytes += bytes_transferred;

		CTcpClientDlg *pdlg = (CTcpClientDlg*)theApp.GetMainWnd();
		if(pdlg->NeedShowRecv())
			pdlg->ShowRecvData(m_buf, bytes_transferred);
		//
		boost::chrono::milliseconds dur = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now() - m_starttime);
		m_rdwrdelay = dur.count();
		//
		m_sock.async_read_some(mutable_buffers_1(m_buf, sizeof(m_buf)),
			boost::bind(&tcp_client::read_handler, this, _1, _2));
	}
}

void tcp_client::write_handler(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
	if(ec)
	{
		LOG_INFO(ec.message());
		++m_errcount;
		reconnect();
	}
	else
	{
		m_writebytes += bytes_transferred;
		//
//		m_sock.async_read_some(const_buffers_1(m_buf, sizeof(m_buf)),
//			boost::bind(&tcp_client::read_handler, this, _1, _2));
	}
}

void tcp_client::send(const void *data, size_t size)
{
	if(!m_connected)
		return;
	m_starttime = boost::chrono::system_clock::now();
	m_sock.async_write_some(const_buffers_1(data, size),
		boost::bind(&tcp_client::write_handler, this, _1, _2));
}

void tcp_client::reconnect_handler(const boost::system::error_code &ec)
{
	if(ec)
	{
		LOG_INFO(ec.message());
	}
	else
	{
		connect();
	}

}

void tcp_client::close()
{
	m_sock.close();
	m_connected = false;
}

void tcp_client::reconnect()
{
	close();
	boost::system::error_code ec;
	m_rctimer.expires_from_now(boost::posix_time::seconds(m_reconn_time), ec);
	if(ec)
	{
		LOG_INFO(ec.message());
		return;
	}
	m_rctimer.async_wait(boost::bind(&tcp_client::reconnect_handler, this, _1));
	m_reconn_time *= 2;
}

tcp_client::~tcp_client(void)
{
	close();
}
