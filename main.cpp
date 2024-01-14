//****************************************************************************
// mongooseDemo1
// James W. Ellert
//****************************************************************************
#include <iostream>
#include <iomanip>
#include <string>
#include "mongoose.h"


using namespace std;
static constexpr const char* HDR_CTFORM = "Content-Type: application/x-www-form-urlencoded\r\n";
static mg_mgr g_mgMgr;

struct Tx {
	string  m_method;
	string  m_url;
	string  m_headers;
	string  m_body;
	string* m_responseBuf;
	int32_t m_respCode;
};
static Tx g_tx;


static void
mongooseClientCallback (mg_connection* nc, int ev, void* evData) {
	switch (ev) {
	case MG_EV_READ: {
		cout << "mongooseClientCallback::MG_EV_READ" << endl;
		//string buf ((char*)nc->recv.buf, nc->recv.len);
		//cout << buf << endl;
		} break;

	case MG_EV_CONNECT: {
		cout << "mongooseClientCallback::MG_EV_CONNECT" << endl;
		auto tx = (Tx*)nc->fn_data;
		mg_tls_opts opts = { 0 };
		mg_tls_init (nc, &opts);
		mg_str host = mg_url_host (tx->m_url.c_str());
		string uri  = mg_url_uri  (tx->m_url.c_str());
		string msg;
		msg = tx->m_method + " ";
		msg += uri + " HTTP/1.1\r\n";
		msg += "Host: " + string(host.ptr, host.len) + "\r\n";
		msg += "Content-Length: " + to_string(tx->m_body.length()) + "\r\n";
		msg += tx->m_headers + "\r\n";
		msg += tx->m_body + "\r\n";
		msg += "\r\n";

		mg_send (nc, msg.c_str(), msg.length());
		} break;

	case MG_EV_HTTP_MSG: {
		cout << "mongooseClientCallback::MG_EV_HTTP_MSG" << endl;
		auto tx = (Tx*)nc->fn_data;
		auto hm = (mg_http_message*)evData;
		tx->m_responseBuf->append (hm->body.ptr, hm->body.len);
		tx->m_respCode = mg_http_status(hm);
		} break;
	}
}


static int32_t
post (const string& url, const string& headers, const string& body, string* response) {
	g_tx.m_method      = "POST";
	g_tx.m_url         = url;
	g_tx.m_headers     = headers;
	g_tx.m_body        = body;
	g_tx.m_responseBuf = response;
	g_tx.m_respCode    = 0;
	mg_http_connect (&g_mgMgr, url.c_str(), mongooseClientCallback, &g_tx);

	for (;;) {
		mg_mgr_poll (&g_mgMgr, 200);
		if (g_tx.m_respCode != 0)
			return g_tx.m_respCode;
	}

	return g_tx.m_respCode;
}

static int32_t
get (const string& url, const string& headers, string* response) {
	g_tx.m_method      = "GET";
	g_tx.m_url         = url;
	g_tx.m_headers     = headers;
	g_tx.m_responseBuf = response;
	g_tx.m_respCode    = 0;
	mg_http_connect (&g_mgMgr, url.c_str(), mongooseClientCallback, &g_tx);

	for (;;) {
		mg_mgr_poll (&g_mgMgr, 200);
		if (g_tx.m_respCode != 0)
			return g_tx.m_respCode;
	}

	return g_tx.m_respCode;
}


static void
testPost() {
	string body = "Hello from mongooseDemo1.";
	string response;
	auto respCode = post ("https://httpbin.org/post", HDR_CTFORM, body, &response);
	if (respCode != 200 || response.find(body) == string::npos)
		cout << "testPost failed." << endl;
	else
		cout << "testPost successful." << endl;

	//cout << response << endl;
}

static void
testBigGet() {
	string response;
	auto respCode = get ("https://httpbin.org/bytes/8000", "", &response);
	if (respCode != 200 || response.length() < 8000)
		cout << "testBigGet failed." << endl;
	else
		cout << "testBigGet successful." << endl;

	//cout << std::hex;
	//for (int i = 0; i < response.length(); ++i)
	//	cout << std::setw(2) << std::setfill('0') << (int)response.at(i);
}


static void
testManyPosts() {
	for (int32_t i = 0; i < 20; ++i)
		testPost();
}

static void
printCmds() {
	cout << "Options:" << endl;
	cout << "         1  :  testPost."      << endl;
	cout << "         2  :  testBigGet."    << endl;
	cout << "         3  :  testManyPosts." << endl;
}


int main (int argc, char** argv) {
	if (argc < 2) {
		printCmds();
		return (0);
	}

	mg_mgr_init(&g_mgMgr);

	int32_t cmd = atoi(argv[1]);

	switch (cmd) {
	case 1:  testPost      (); break;
	case 2:  testBigGet    (); break;
	case 3:  testManyPosts (); break;
	default: printCmds     (); return (0);
	}

	mg_mgr_free(&g_mgMgr);

	cout << endl << "====Test complete====" << endl;
}

