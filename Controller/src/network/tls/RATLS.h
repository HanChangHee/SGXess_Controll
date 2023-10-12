
#ifndef RATLS_H_
#define RATLS_H_

#include <string>

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"

class RATLS {
public:
	RATLS();
	virtual ~RATLS();

	static bool getAttestType(std::string &type);

	static bool getAttestLib(void **attestLib, int (**createKey)(uint8_t** key, size_t* key_size, uint8_t** crt, size_t* crt_size));

	static bool setCertificate(mbedtls_x509_crt &srvcert, mbedtls_pk_context &pkey, mbedtls_ctr_drbg_context &ctr_drbg,
			int (*createkey)(uint8_t** key, size_t* key_size, uint8_t** crt, size_t* crt_size));

private:
	static size_t file_read(const char* path, char* buf, size_t count);
};

#endif /* RATLS_H_ */
