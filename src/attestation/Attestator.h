#ifndef ATTESTATOR_H_
#define ATTESTATOR_H_

#include <string>
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/x509_crt.h"

#define mbedtls_fprintf fprintf
#define mbedtls_printf printf

#define MBEDTLS_EXIT_SUCCESS EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE EXIT_FAILURE

#define SERVER_NAME "localhost"
#define CA_CRT_PATH "ssl/ca.crt"
#define SRV_CRT_PATH "ssl/server.crt"
#define SRV_KEY_PATH "ssl/server.key"

#define MALICIOUS_STR "MALICIOUS DATA"

#define HTTP_RESPONSE                                    \
    "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" \
    "<h2>mbed TLS Test Server</h2>\r\n"                  \
    "<p>Successful connection using: %s</p>\r\n"

#define DEBUG_LEVEL 0

class Attestator {
public:
	Attestator();
	virtual ~Attestator();

	bool initRATLS();
	bool setupRATLS(unsigned short port);
	bool getClientMsg(std::string &msg);

private:
	bool getAttestType(std::string &type);
	bool loadAttestLibrary(std::string &attestType, void* &ra_tls_attest_lib);
	bool generateKeyRATLS(const void *ra_tls_attest_lib, mbedtls_x509_crt& srvcert, mbedtls_pk_context& pkey);
	bool generateKey(const void *ra_tls_attest_lib, mbedtls_x509_crt& srvcert, mbedtls_pk_context& pkey);
	bool seedRand();

private:
	const static std::string RATLS_KEY_FILE_PATH;
	const static std::string RATLS_CERT_FILE_PATH;

	void *m_ra_tls_attest_lib;
	std::string m_attestType;
	mbedtls_ctr_drbg_context m_ctr_drbg;

    mbedtls_x509_crt m_srvcert;
    mbedtls_pk_context m_pkey;

    mbedtls_net_context m_listen_fd;
    mbedtls_ssl_context m_ssl;
    mbedtls_ssl_config m_conf;
};

#endif /* ATTESTATOR_H_ */
