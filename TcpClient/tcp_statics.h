#pragma once

class tcp_statics
{
public:
	tcp_statics(void);
	~tcp_statics(void);
	
	bool is_connected() const {return m_connected; }
	int get_errcount() const{return m_errcount; }
	int get_readbytes() const{return m_readbytes; }
	int get_writebytes() const{return m_writebytes; }
	int get_conndelay() const{return m_conndelay; }
	int get_rdwrdelay() const{return m_rdwrdelay; }

protected:	
	bool m_connected;
	int m_errcount;
	int m_readbytes;
	int m_writebytes;
	int m_conndelay;
	int m_rdwrdelay;
};
