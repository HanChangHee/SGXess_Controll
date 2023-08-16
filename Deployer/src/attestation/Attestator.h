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

#define CA_CRT_PATH "ssl/ca.crt"

#define GET_REQUEST "GET / HTTP/1.0\r\n\r\n"

#define DEBUG_LEVEL 0

class Attestator {
public:
	Attestator();
	virtual ~Attestator();

	bool sendMsg(std::string &msg, std::string &ip, unsigned int port);

private:
	int parse_hex(const char* hex, void* buffer, size_t buffer_size);
	bool getenv_client_inside_sgx();
	bool getAttestType(std::string &type);
	bool seedRand();
	static int my_verify_callback(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags);

private:

	mbedtls_ctr_drbg_context m_ctr_drbg;
};

#endif /* ATTESTATOR_H_ */
