//****************************************************************************
// mongooseDemo1
// James W. Ellert
//****************************************************************************
#include <iostream>
#include <iomanip>
#include <string>
#include "mongoose.h"


using namespace std;
static const char* s_url  = "https://httpbin.org/post";
static const char* s_body = "Hello from mongooseDemo:";
static mg_mgr g_mgMgr;



struct Msg {
	int32_t m_tryNum;
	string  m_response;
	int32_t m_respCode;
};


static void
fn (mg_connection* nc, int ev, void* ev_data) {
	switch (ev) {
	case MG_EV_READ: {
		cout << "-fn::MG_EV_READ" << endl;
		//string buf ((char*)nc->recv.buf, nc->recv.len);
		//cout << buf << endl;
		} break;

	case MG_EV_CONNECT: {
		cout << endl << "-fn::MG_EV_CONNECT" << endl;
		mg_tls_opts opts = { 0 };
		mg_tls_init (nc, &opts);

		Msg*   msg     = (Msg*)nc->fn_data;
		mg_str host    = mg_url_host (s_url);
		int    bodyLen = (int)strlen(s_body) + ((msg->m_tryNum < 10) ? 1 : 2);

		mg_printf (nc, "POST %s HTTP/1.0\r\n"
			"Host: %.*s\r\n"
			"Content-Length: %d\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n\r\n"
			"%s%d\r\n"
			"\r\n",
			mg_url_uri(s_url), (int)host.len, host.ptr, bodyLen, s_body, msg->m_tryNum);
		} break;

	case MG_EV_HTTP_MSG: {
		cout << "-fn::MG_EV_HTTP_MSG" << endl;
		Msg* msg = (Msg*)nc->fn_data;
		auto hm = (mg_http_message*)ev_data;
		msg->m_response.append (hm->body.ptr, hm->body.len);
		msg->m_respCode = mg_http_status(hm);
		} break;

	case MG_EV_ERROR:
		cout << "-fn:MG_EV_ERROR: " << (char*)ev_data << endl;
		break;
	}
}



int main (int argc, char** argv) {
	mg_mgr_init (&g_mgMgr);

	for (int32_t i = 0; i < 4; ++i) {
		Msg msg;
		msg.m_tryNum   = i;
		msg.m_respCode = 0;

		mg_http_connect (&g_mgMgr, s_url, fn, (void*)&msg);

		for (;;) {
			mg_mgr_poll (&g_mgMgr, 200);
			if (msg.m_respCode != 0)
				break;
		}

		if (msg.m_respCode != 200 || msg.m_response.find(s_body) == string::npos) {
			cout << "-----Test Post failed:" << endl;
			cout << "respCode: " << msg.m_respCode << endl;
			cout << "response: \"" << msg.m_response.c_str() << "\"" << endl;
		} else {
			cout << "-----Test Post successful." << endl;
		}
	}

	mg_mgr_free (&g_mgMgr);
	cout << endl << "-----Test complete." << endl;
}

