//****************************************************************************
// mongooseDemo1
// James W. Ellert
//****************************************************************************
#include <string.h>
#include <stdio.h>
#include "mongoose.h"


static const char*   s_url  = "https://httpbin.org/post";
static const char*   s_body = "Hello from mongooseDemo:";
static struct mg_mgr g_mgMgr;



struct Msg {
	int  m_tryNum;
	char m_response[1024];
	int  m_respCode;
};


static void
fn (struct mg_connection* nc, int ev, void* ev_data) {
	struct Msg* msg = (struct Msg*)nc->fn_data;

	switch (ev) {
	case MG_EV_READ: {
		printf ("-fn::MG_EV_READ\n");
		//string buf ((char*)nc->recv.buf, nc->recv.len);
		//cout << buf << endl;
		} break;

	case MG_EV_CONNECT: {
		printf ("\n-fn::MG_EV_CONNECT\n");
		struct mg_tls_opts opts = { 0 };
		mg_tls_init (nc, &opts);

		struct mg_str host    = mg_url_host (s_url);
		int           bodyLen = (int)strlen(s_body) + ((msg->m_tryNum < 10) ? 1 : 2);

		mg_printf (nc, "POST %s HTTP/1.0\r\n"
			"Host: %.*s\r\n"
			"Content-Length: %d\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n\r\n"
			"%s%d\r\n"
			"\r\n",
			mg_url_uri(s_url), (int)host.len, host.ptr, bodyLen, s_body, msg->m_tryNum);
		} break;

	case MG_EV_HTTP_MSG: {
		printf ("-fn::MG_EV_HTTP_MSG\n");
		struct mg_http_message* hm = (struct mg_http_message*)ev_data;
		strncpy_s (msg->m_response, sizeof(msg->m_response), hm->body.ptr, hm->body.len);
		msg->m_response[sizeof(msg->m_response)-1] = 0;
		msg->m_respCode = mg_http_status(hm);
		} break;

	case MG_EV_ERROR:
		printf ("-fn:MG_EV_ERROR: %s\n", (char*)ev_data);
		break;
	}
}



int main (int argc, char** argv) {
	mg_mgr_init (&g_mgMgr);

	for (int32_t i = 0; i < 20; ++i) {
		struct Msg msg;
		msg.m_tryNum   = i;
		msg.m_respCode = 0;

		mg_http_connect (&g_mgMgr, s_url, fn, (void*)&msg);

		for (;;) {
			mg_mgr_poll (&g_mgMgr, 200);
			if (msg.m_respCode != 0)
				break;
		}

		if (msg.m_respCode != 200 || !strstr (msg.m_response, s_body)) {
			printf ("-----Test Post failed:\n");
			printf ("respCode: %d\n", msg.m_respCode);
			printf ("response: \"%s\"\n", msg.m_response);
		} else {
			printf ("-----Test Post successful.\n");
		}
	}

	mg_mgr_free (&g_mgMgr);
	printf ("\n-----Test complete.\n");
}

