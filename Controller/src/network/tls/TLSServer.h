
#ifndef TLSSERVER_H_
#define TLSSERVER_H_

#include <string>

#include "mbedtls/ssl_cookie.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/timing.h"

#define SRV_CRT_PATH "ssl/server.crt"
#define SRV_KEY_PATH "ssl/server.key"

class TLSServer {
public:
	TLSServer();
	virtual ~TLSServer();

	void terminate();

	bool setListener(std::string port, bool bRATLS = false);

	bool acceptClient();

	void resetClient();

	bool recvMsg(std::string &msg);

	bool sendMsg(std::string msg);

private:
	void init();

	void destroy();

	bool loadCertificate();

	bool setSSLConfig();

private:
	bool m_RATLS;

	mbedtls_net_context m_server;
	mbedtls_net_context m_client;
	mbedtls_entropy_context m_entropy;
	mbedtls_ctr_drbg_context m_ctr_drbg;
	mbedtls_ssl_context m_ssl;
	mbedtls_ssl_config m_conf;
	mbedtls_x509_crt m_srvcert;
	mbedtls_pk_context m_pkey;
	mbedtls_timing_delay_context m_timer;
};

#endif /* TLSSERVER_H_ */
