#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"

#include "Attestator.h"

/* RA-TLS: on server, only need ra_tls_create_key_and_crt_der() to create keypair and X.509 cert */
int (*ra_tls_create_key_and_crt_der_f)(u_int8_t** der_key, size_t* der_key_size, u_int8_t** der_crt, size_t* der_crt_size);

Attestator::Attestator() {
	m_ra_tls_attest_lib = NULL;
    mbedtls_x509_crt_init(&m_srvcert);
    mbedtls_pk_init(&m_pkey);

    mbedtls_ctr_drbg_init(&m_ctr_drbg);

    mbedtls_net_init(&m_listen_fd);

    mbedtls_ssl_init(&m_ssl);
    mbedtls_ssl_config_init(&m_conf);
}

Attestator::~Attestator() {
    mbedtls_x509_crt_free(&m_srvcert);
    mbedtls_pk_free(&m_pkey);

    mbedtls_ctr_drbg_free(&m_ctr_drbg);

    mbedtls_net_free(&m_listen_fd);

    mbedtls_ssl_free(&m_ssl);
    mbedtls_ssl_config_free(&m_conf);

    if ( NULL != m_ra_tls_attest_lib ) {
    	dlclose(m_ra_tls_attest_lib);
    	m_ra_tls_attest_lib = NULL;
    }
}

bool Attestator::initRATLS() {
	// get attestation type
    if ( false == getAttestType(m_attestType) ) {
    	return false;
    }

    // load library depend on attestation type
    m_ra_tls_attest_lib = NULL;
    if ( false == loadAttestLibrary(m_attestType, m_ra_tls_attest_lib) ) {
    	return false;
    }

    // seed random number
    if ( false == seedRand() ) {
    	return false;
    }

    // generate key
    if ( false == generateKey(m_ra_tls_attest_lib, m_srvcert, m_pkey) ) {
    	return false;
    }

    return true;
}

bool Attestator::setupRATLS(unsigned short port) {
	bool ret = false;

    // bind
    mbedtls_printf("  . Bind on https://localhost:%u/ ...", port);
    fflush(stdout);

    do {
        if ( 0 != mbedtls_net_bind(&m_listen_fd, NULL, std::to_string(port).c_str(), MBEDTLS_NET_PROTO_TCP) ) {
            mbedtls_printf(" failed\n  ! mbedtls_net_bind returned\n\n");
            break;
        }
        mbedtls_printf(" ok\n");

        mbedtls_printf("  . Setting up the SSL data....");
        fflush(stdout);

        // set configuration
        if ( 0 != mbedtls_ssl_config_defaults(&m_conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT) ) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned\n\n");
            break;
        }

        mbedtls_ssl_conf_rng(&m_conf, mbedtls_ctr_drbg_random, &m_ctr_drbg);
        //mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

        if ( NULL == m_ra_tls_attest_lib ) {
            /* no RA-TLS attest library present, use embedded CA chain */
            mbedtls_ssl_conf_ca_chain(&m_conf, m_srvcert.next, NULL);
        }

        if ( 0 != mbedtls_ssl_conf_own_cert(&m_conf, &m_srvcert, &m_pkey) ) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned\n\n");
            break;
        }

        if ( 0 != mbedtls_ssl_setup(&m_ssl, &m_conf) ) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned\n\n");
            break;
        }

        mbedtls_printf(" ok\n");
        ret = true;
    } while(false);

    return ret;
}

/*
static void my_debug(void* ctx, int level, const char* file, int line, const char* str) {
    ((void)level);

    mbedtls_fprintf((FILE*)ctx, "%s:%04d: %s\n", file, line, str);
    fflush((FILE*)ctx);
}
*/
bool Attestator::getAttestType(std::string &type) {
    // check attestation type
    char attestation_type_str[32] = {0};
    std::string filename = "/dev/attestation/attestation_type";

    FILE *f = fopen(filename.c_str(), "r");
    if ( NULL == f ) {
    	printf("Failed to open file %s\n", filename.c_str());
    	return false;
    }

    ssize_t bytes = fread(attestation_type_str, 1, 32, f);
    if (bytes <= 0) {
        int errsv = errno;
        return -errsv;
    }

    if ( bytes <= 0 ) {
        fclose(f);
        mbedtls_printf("User requested RA-TLS attestation but cannot read SGX-specific file "
                       "/dev/attestation/attestation_type\n");
        return false;
    }
    else {
       fclose(f);
    	type = std::string(attestation_type_str);
    }

    return true;
}

bool Attestator::loadAttestLibrary(std::string &attestType, void* &ra_tls_attest_lib) {
	if ( 0 == attestType.compare("none") ) {
        ra_tls_attest_lib = NULL;
        ra_tls_create_key_and_crt_der_f = NULL;
	}
	else if ( 0 == attestType.compare("epid") || 0 == attestType.compare("dcap") ) {
	    ra_tls_attest_lib = dlopen("libra_tls_attest.so", RTLD_LAZY);
	    if ( NULL == ra_tls_attest_lib ) {
	        mbedtls_printf("User requested RA-TLS attestation but cannot find lib\n");
	        return false;
	    }

	    char* error = NULL;
	    ra_tls_create_key_and_crt_der_f = dlsym(ra_tls_attest_lib, "ra_tls_create_key_and_crt_der");
	    if ( NULL != (error = dlerror()) ) {
	    	mbedtls_printf("%s\n", error);
	    }
	}
	else {
        mbedtls_printf("Unrecognized remote attestation type: %s\n", attestType.c_str());
        return false;
    }

	return true;
}

bool Attestator::generateKeyRATLS(const void *ra_tls_attest_lib, mbedtls_x509_crt& srvcert, mbedtls_pk_context& pkey) {
    mbedtls_printf("\n  . Creating the RA-TLS server cert and key (using \"%s\" as "
                   "attestation type)...", m_attestType.c_str());
    fflush(stdout);

    uint8_t* der_key = NULL;
    uint8_t* der_crt = NULL;
    size_t der_key_size;
    size_t der_crt_size;

    bool ret = false;
    do {
        if ( 0 != (*ra_tls_create_key_and_crt_der_f)(&der_key, &der_key_size, &der_crt, &der_crt_size) ) {
            mbedtls_printf(" failed\n  !  ra_tls_create_key_and_crt_der returned n\n");
            break;
         }

        if ( 0 != mbedtls_x509_crt_parse(&srvcert, (unsigned char*)der_crt, der_crt_size) ) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned\n\n");
            break;
         }

        if ( 0 != mbedtls_pk_parse_key(&pkey, (unsigned char*)der_key, der_key_size, /*pwd=*/NULL, 0, mbedtls_ctr_drbg_random, &m_ctr_drbg) ) {
            mbedtls_printf(" failed\n  !  mbedtls_pk_parse_key returned\n\n");
            break;
         }

        mbedtls_printf(" ok\n");

        // test malicious quote process
        /* user asks to maliciously modify the embedded SGX quote (for testing purposes) */
//        mbedtls_printf("  . Maliciously modifying SGX quote embedded in RA-TLS cert...");
//        fflush(stdout);
//
//        uint8_t oid[] = {0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF8, 0x4D, 0x8A, 0x39, 0x06};
//        uint8_t* p = memmem(srvcert.v3_ext.p, srvcert.v3_ext.len, oid, sizeof(oid));
//
//        if ( NULL == p ) {
//            mbedtls_printf(" failed\n  !  No embedded SGX quote found\n\n");
//            break;
//         }
//
//        p += sizeof(oid);
//        p += 5; /* jump somewhere in the middle of the SGX quote */
//        if (p + sizeof(MALICIOUS_STR) > srvcert.v3_ext.p + srvcert.v3_ext.len) {
//            mbedtls_printf(" failed\n  !  Size of embedded SGX quote is too small\n\n");
//            break;
//        }
//
//        memcpy(p, MALICIOUS_STR, sizeof(MALICIOUS_STR));
//        mbedtls_printf(" ok\n");

        ret = true;
    } while(false);

    if ( NULL != der_key ) {
    	free(der_key);
    	der_key = NULL;
    }
    if ( NULL != der_crt ) {
    	free(der_crt);
    	der_crt = NULL;
    }

    return ret;
}

bool Attestator::generateKey(const void *ra_tls_attest_lib, mbedtls_x509_crt& srvcert, mbedtls_pk_context& pkey) {
	if ( NULL != ra_tls_attest_lib ) {
    	return generateKeyRATLS(ra_tls_attest_lib, srvcert, pkey);
    }
    else {
        mbedtls_printf("\n  . Creating normal server cert and key...");
        fflush(stdout);

        if ( 0 != mbedtls_x509_crt_parse_file(&srvcert, SRV_CRT_PATH) ) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse_file returned\n\n");
            return false;
        }

        if ( 0 != mbedtls_x509_crt_parse_file(&srvcert, CA_CRT_PATH) ) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse_file returned\n\n");
            return false;
        }

        if ( 0 != mbedtls_pk_parse_keyfile(&pkey, SRV_KEY_PATH, /*password=*/NULL, mbedtls_ctr_drbg_random, &m_ctr_drbg) ) {
            mbedtls_printf(" failed\n  !  mbedtls_pk_parse_keyfile returned\n\n");
            return false;
        }

        mbedtls_printf(" ok\n");
        return true;
    }
}

bool Attestator::seedRand() {
    const char* pers = "SGX Access Control Server";
    mbedtls_entropy_context entropy;

    mbedtls_printf("  . Seeding the random number generator...");
    fflush(stdout);

    mbedtls_entropy_init(&entropy);

    int ret = mbedtls_ctr_drbg_seed(&m_ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char*)pers, strlen(pers));
    if ( 0 != ret ) {
        mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned\n");
        return false;
    }

    mbedtls_entropy_free(&entropy);
    return true;
}

bool Attestator::getClientMsg(std::string &msg) {
    mbedtls_net_context client_fd;
    mbedtls_net_init(&client_fd);

    mbedtls_net_free(&client_fd);
    mbedtls_ssl_session_reset(&m_ssl);

    mbedtls_printf("  . Waiting for a remote connection ...");
    fflush(stdout);

    // listen
    if ( 0 != mbedtls_net_accept(&m_listen_fd, &client_fd, NULL, 0, NULL) ) {
        mbedtls_printf(" failed\n  ! mbedtls_net_accept returned\n\n");
        return false;
    }

    mbedtls_ssl_set_bio(&m_ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    mbedtls_printf(" ok\n");

    mbedtls_printf("  . Performing the SSL/TLS handshake...");
    fflush(stdout);

    unsigned int ret = mbedtls_ssl_handshake(&m_ssl);
    while ( ret != 0 ) {
        char error_buf[100] = {0, };
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        mbedtls_printf("Last error was: %s\n\n", error_buf);

    	mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned %x\n\n", ret);
       return false;
    }

    mbedtls_printf(" ok\n");

    // Read message from client
    mbedtls_printf("  < Read from client:");
    fflush(stdout);

    do {
    	unsigned char buf[1024] = {0, };
    	size_t len = sizeof(buf) - 1;
       int ret = mbedtls_ssl_read(&m_ssl, buf, len);
       msg.append((char *)buf);

       len = ret;
       mbedtls_printf(" %lu bytes read\n\n%s", len, (char*)buf);

       if (ret > 0)
           break;

       if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
           continue;

       if (ret <= 0) {
           switch (ret) {
                case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                    mbedtls_printf(" connection was closed gracefully\n");
                    break;

                case MBEDTLS_ERR_NET_CONN_RESET:
                    mbedtls_printf(" connection was reset by peer\n");
                    break;

                default:
                    mbedtls_printf(" mbedtls_ssl_read returned -0x%x\n", -ret);
                    break;
            }
           break;
        }
    } while (true);

    mbedtls_printf("  > Write to client:");
    fflush(stdout);

    unsigned char writeBuf[1024] = {0, };
    size_t writeLen = sprintf((char*)writeBuf, HTTP_RESPONSE, mbedtls_ssl_get_ciphersuite(&m_ssl));
    int writeRet = mbedtls_ssl_write(&m_ssl, writeBuf, writeLen);

    while ( 0 >= writeRet) {
        if (writeRet == MBEDTLS_ERR_NET_CONN_RESET) {
            mbedtls_printf(" failed\n  ! peer closed the connection\n\n");
            return false;
        }

        if (writeRet != MBEDTLS_ERR_SSL_WANT_READ && writeRet != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", writeRet);
            return false;
        }
    }

    writeLen = writeRet;
    mbedtls_printf(" %lu bytes written\n\n%s\n", writeLen, (char*)writeBuf);

    mbedtls_printf("  . Closing the connection...");

    while ( mbedtls_ssl_close_notify(&m_ssl) < 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_close_notify returned\n\n");
        return false;
    }

    mbedtls_printf(" ok\n");

    mbedtls_net_free(&client_fd);

    mbedtls_ssl_session_reset(&m_ssl);

    return true;
}
