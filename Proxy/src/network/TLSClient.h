/*
 * TLSWrapper.h
 *
 *  Created on: 2023. 9. 10.
 *      Author: chhan
 */

#ifndef TLSCLIENT_H_
#define TLSCLIENT_H_

#define READ_TIMEOUT_MS 1000
#define MAX_RETRY       5

#define CA_CRT_PATH "ssl/ca.crt"

#include <string>

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/timing.h"

class TLSClient {
public:
	TLSClient();
	virtual ~TLSClient();

	bool connect(std::string IP, std::string port);

	void terminate();

	bool sendMsg(std::string &msg);

	bool recvMsg(std::string &msg);

private:
	void init();

	void destroy();

	bool setSSLConfig(std::string &IP);

	bool loadCertificate();

	bool verifyCertificate();
	static int my_verify_measurements(const char* mrenclave, const char* mrsigner, const char* isv_prod_id, const char* isv_svn);
	static int my_verify_callback(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags);

private:
	mbedtls_net_context m_server;
    mbedtls_entropy_context m_entropy;
    mbedtls_ctr_drbg_context m_ctr_drbg;
    mbedtls_ssl_context m_ssl;
    mbedtls_ssl_config m_conf;
    mbedtls_x509_crt m_cacert;
    mbedtls_timing_delay_context m_timer;


};

#endif /* TLSCLIENT_H_ */
