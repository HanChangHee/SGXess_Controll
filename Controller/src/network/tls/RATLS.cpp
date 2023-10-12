
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "mbedtls/x509.h"

#include "RATLS.h"

RATLS::RATLS() {
}

RATLS::~RATLS() {
}

bool RATLS::setCertificate(mbedtls_x509_crt &srvcert, mbedtls_pk_context &pkey, mbedtls_ctr_drbg_context &ctr_drbg,
		int (*createkey)(uint8_t** key, size_t* key_size, uint8_t** crt, size_t* crt_size)) {
	int ret = 0;
	uint8_t *der_key = NULL;
	uint8_t *der_crt = NULL;

	size_t der_key_size = 0;
	size_t der_crt_size = 0;

	printf("\n  . Creating the RA-TLS server cert and key...");
	fflush(stdout);

	ret = (*createkey)(&der_key, &der_key_size, &der_crt, &der_crt_size);
	if (ret != 0) {
		printf(" failed\n  !  ra_tls_create_key_and_crt_der returned %d\n\n", ret);
		return false;
	}

	ret = mbedtls_x509_crt_parse(&srvcert, (unsigned char*)der_crt, der_crt_size);
	if (ret != 0) {
		printf(" failed\n  !  mbedtls_x509_crt_parse returned %d\n\n", ret);
		return false;
	}

	ret = mbedtls_pk_parse_key(&pkey, (unsigned char*)der_key, der_key_size, /*pwd=*/NULL, 0,
                               mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		printf(" failed\n  !  mbedtls_pk_parse_key returned %d\n\n", ret);
		return false;
	}

	// TODO: test malicious quote

	printf(" ok\n");
	return true;
}

bool RATLS::getAttestLib(void **attestLib, int (**createKey)(uint8_t** key, size_t* key_size, uint8_t** crt, size_t* crt_size)) {
    *attestLib = dlopen("libra_tls_attest.so", RTLD_LAZY);
    if ( NULL == *attestLib ) {
        printf("User requested RA-TLS attestation but cannot find lib\n");
        return false;
    }

    char* error;
    *createKey = dlsym(*attestLib, "ra_tls_create_key_and_crt_der");
    if ( NULL != (error = dlerror())) {
        printf("%s\n", error);
        return false;
    }

    return true;
}


bool RATLS::getAttestType(std::string &type) {
	int ret = 0;
    char attestation_type_str[32] = {0};
    ret = file_read("/dev/attestation/attestation_type", attestation_type_str,
                    sizeof(attestation_type_str) - 1);
    if (ret < 0 && ret != -ENOENT) {
        printf("User requested RA-TLS attestation but cannot read SGX-specific file "
                       "/dev/attestation/attestation_type\n");
        return false;
    }

    type.append(attestation_type_str);
    return true;
}

size_t RATLS::file_read(const char* path, char* buf, size_t count) {
    FILE* f = fopen(path, "r");
    if (!f)
        return -errno;

    size_t bytes = fread(buf, 1, count, f);
    if (bytes <= 0) {
        int errsv = errno;
        fclose(f);
        return -errsv;
    }

    int close_ret = fclose(f);
    if (close_ret < 0)
        return -errno;

    return bytes;
}
