#ifndef VLC_NET_DATA_H
#define VLC_NET_DATA_H 1

enum NetDataEvent
{
	NET_DATA_ELE_START = 0,
	NET_DATA_DNS_START,
	NET_DATA_DNS_END,
	NET_DATA_TCP_START,
	NET_DATA_TCP_END,
	NET_DATA_TLS_START,
	NET_DATA_TLS_END,
	NET_DATA_SEND_START,
	NET_DATA_SEND_HEADER,
	NET_DATA_SEND_END,
	NET_DATA_RECV_START,
	NET_DATA_RECV_HEADER,
	NET_DATA_RECV_DATA,
	NET_DATA_RECV_END,
};

struct NetData
{
	enum NetDataEvent event_type_;		// ÊÂŒþÀàÐÍ
	unsigned long long id;			// Ë÷Òýid
	unsigned long long begin_time;		// ²ÉŒ¯µœµÄÊ±Œä1
	unsigned long long end_time;		// ²ÉŒ¯µœµÄÊ±Œä1
	unsigned long ip;			// Ä¿±êip
	unsigned long data_length;
	const char * data;
};

struct CustomHeader
{
	char key[64];
	char value[128];
};

#endif
