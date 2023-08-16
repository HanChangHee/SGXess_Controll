/*
 * Listener.cpp
 *
 *  Created on: 2023. 4. 10.
 *      Author: chhan
 */
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/build_info.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"

#include "gramine/ra_tls.h"

#include "Attestator.h"


/* RA-TLS: on client, only need to register ra_tls_verify_callback_extended_der() for cert * verification. */
int (*ra_tls_verify_callback_extended_der_f)(uint8_t* der_crt, size_t der_crt_size, struct ra_tls_verify_callback_results* results);

/* RA-TLS: if specified in command-line options, use our own callback to verify SGX measurements */
void (*ra_tls_set_measurement_callback_f)(int (*f_cb)(const char* mrenclave, const char* mrsigner, const char* isv_prod_id, const char* isv_svn));

Attestator::Attestator() {
    mbedtls_ctr_drbg_init(&m_ctr_drbg);
}

Attestator::~Attestator() {
    mbedtls_ctr_drbg_free(&m_ctr_drbg);
}


bool Attestator::sendMsg(std::string &msg, std::string &ip, unsigned int port) {
    int ret;
    size_t len;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    mbedtls_net_context server_fd;
    uint32_t flags;
    unsigned char buf[1024];
    const char* pers = "ssl_client1";
    bool in_sgx = getenv_client_inside_sgx();

    void* ra_tls_verify_lib = NULL;
    ra_tls_verify_callback_extended_der_f = NULL;
    ra_tls_set_measurement_callback_f = NULL;
    struct ra_tls_verify_callback_results my_verify_callback_results;

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);

    // get remote attestation type
    std::string attestType;
    getAttestType(attestType);

    // get attestation library
    if ( 0 == attestType.compare("epid") ) {
        ra_tls_verify_lib = dlopen("libra_tls_verify_epid.so", RTLD_LAZY);
        if ( NULL == ra_tls_verify_lib ) {
            mbedtls_printf("%s\n", dlerror());
            mbedtls_printf("User requested RA-TLS verification with EPID but cannot find lib\n");
            if ( true == in_sgx ) {
                mbedtls_printf("Please make sure that you are using client_epid.manifest\n");
            }
            return 1;
        }
    } else if ( 0 == attestType.compare("dcap") ) {
        if ( true == in_sgx ) {
            /*
             * RA-TLS verification with DCAP inside SGX enclave uses dummies instead of real
             * functions from libsgx_urts.so, thus we don't need to load this helper library.
             */
            ra_tls_verify_lib = dlopen("libra_tls_verify_dcap_gramine.so", RTLD_LAZY);
            if ( NULL == ra_tls_verify_lib ) {
                mbedtls_printf("%s\n", dlerror());
                mbedtls_printf("User requested RA-TLS verification with DCAP inside SGX but cannot find lib\n");
                mbedtls_printf("Please make sure that you are using client_dcap.manifest\n");
                return 1;
            }
        } else {
            void* helper_sgx_urts_lib = dlopen("libsgx_urts.so", RTLD_NOW | RTLD_GLOBAL);
            if ( NULL == helper_sgx_urts_lib ) {
                mbedtls_printf("%s\n", dlerror());
                mbedtls_printf("User requested RA-TLS verification with DCAP but cannot find helper"
                               " libsgx_urts.so lib\n");
                return 1;
            }

            ra_tls_verify_lib = dlopen("libra_tls_verify_dcap.so", RTLD_LAZY);
            if ( NULL == ra_tls_verify_lib ) {
                mbedtls_printf("%s\n", dlerror());
                mbedtls_printf("User requested RA-TLS verification with DCAP but cannot find lib\n");
                return 1;
            }
        }
    }

    // measurement callback function define
    char* error = NULL;
    if ( NULL != ra_tls_verify_lib ) {
        ra_tls_set_measurement_callback_f = dlsym(ra_tls_verify_lib, "ra_tls_set_measurement_callback");
        if ( NULL != (error = dlerror()) ) {
            mbedtls_printf("%s\n", error);
            return 1;
        }

        ra_tls_verify_callback_extended_der_f = dlsym(ra_tls_verify_lib, "ra_tls_verify_callback_extended_der");
        if ( NULL != (error = dlerror())) {
            mbedtls_printf("%s\n", error);
            return 1;
        }
    }

    // set attestation arguments
/*    if (argc > 2 && ra_tls_verify_lib) {
        if (argc != 6) {
            mbedtls_printf("USAGE: %s %s <expected mrenclave> <expected mrsigner>"
                           " <expected isv_prod_id> <expected isv_svn>\n"
                           "       (first two in hex, last two as decimal; set to 0 to ignore)\n",
                           argv[0], argv[1]);
            return 1;
        }
    size:        0x10000000
    edmm:        False
    max_threads: 4
    isv_prod_id: 0
    isv_svn:     0
    mr_enclave:   b8e5cbca355f7acba0502abdc803145f8173b5a88ad743d128d8237cd88a2071
    attr.flags:  0x6
    attr.xfrm:   0x3

        mbedtls_printf("[ using our own SGX-measurement verification callback"
                       " (via command line options) ]\n");

        g_verify_mrenclave   = true;
        g_verify_mrsigner    = true;
        g_verify_isv_prod_id = true;
        g_verify_isv_svn     = true;

        (*ra_tls_set_measurement_callback_f)(my_verify_measurements);

        if (!strcmp(argv[2], "0")) {
            mbedtls_printf("  - ignoring MRENCLAVE\n");
            g_verify_mrenclave = false;
        } else if (parse_hex(argv[2], g_expected_mrenclave, sizeof(g_expected_mrenclave)) < 0) {
            mbedtls_printf("Cannot parse MRENCLAVE!\n");
            return 1;
        }

        if (!strcmp(argv[3], "0")) {
            mbedtls_printf("  - ignoring MRSIGNER\n");
            g_verify_mrsigner = false;
        } else if (parse_hex(argv[3], g_expected_mrsigner, sizeof(g_expected_mrsigner)) < 0) {
            mbedtls_printf("Cannot parse MRSIGNER!\n");
            return 1;
        }

        if (!strcmp(argv[4], "0")) {
            mbedtls_printf("  - ignoring ISV_PROD_ID\n");
            g_verify_isv_prod_id = false;
        } else {
            errno = 0;
            uint16_t isv_prod_id = (uint16_t)strtoul(argv[4], NULL, 10);
            if (errno) {
                mbedtls_printf("Cannot parse ISV_PROD_ID!\n");
                return 1;
            }
            memcpy(g_expected_isv_prod_id, &isv_prod_id, sizeof(isv_prod_id));
        }

        if (!strcmp(argv[5], "0")) {
            mbedtls_printf("  - ignoring ISV_SVN\n");
            g_verify_isv_svn = false;
        } else {
            errno = 0;
            uint16_t isv_svn = (uint16_t)strtoul(argv[5], NULL, 10);
            if (errno) {
                mbedtls_printf("Cannot parse ISV_SVN\n");
                return 1;
            }
            memcpy(g_expected_isv_svn, &isv_svn, sizeof(isv_svn));
        }
    } else if (ra_tls_verify_lib) {
        mbedtls_printf("[ using default SGX-measurement verification callback"
                       " (via RA_TLS_* environment variables) ]\n");
        (*ra_tls_set_measurement_callback_f)(NULL);  just to test RA-TLS code
    } else {
        mbedtls_printf("[ using normal TLS flows ]\n");
    }
*/
    mbedtls_printf("\n  . Seeding the random number generator...");
    fflush(stdout);

    seedRand();

    mbedtls_printf("  . Connecting to tcp/%s/%u...", ip.c_str(), port);
    fflush(stdout);

    ret = mbedtls_net_connect(&server_fd, ip.c_str(), std::to_string(port).c_str(), MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    mbedtls_printf("  . Setting up the SSL/TLS structure...");
    fflush(stdout);

    ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    mbedtls_printf("  . Loading the CA root certificate ...");
    fflush(stdout);

    ret = mbedtls_x509_crt_parse_file(&cacert, CA_CRT_PATH);
    if (ret < 0) {
        mbedtls_printf( " failed\n  !  mbedtls_x509_crt_parse_file returned -0x%x\n\n", -ret );
        goto exit;
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_printf(" ok\n");

    if (ra_tls_verify_lib) {
        /* use RA-TLS verification callback; this will overwrite CA chain set up above */
        mbedtls_printf("  . Installing RA-TLS callback ...");
        mbedtls_ssl_conf_verify(&conf, &my_verify_callback, &my_verify_callback_results);
        mbedtls_printf(" ok\n");
    }

    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &m_ctr_drbg);
    //mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

    ret = mbedtls_ssl_setup(&ssl, &conf);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    ret = mbedtls_ssl_set_hostname(&ssl, ip.c_str());
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    mbedtls_printf("  . Performing the SSL/TLS handshake...");
    fflush(stdout);

    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n", -ret);

            char error_buf[100] = {0, };
            mbedtls_strerror(ret, error_buf, sizeof(error_buf));
            mbedtls_printf("Last error was: %s\n\n", error_buf);

            mbedtls_printf("  ! ra_tls_verify_callback_results:\n"
                           "    attestation_scheme=%d, err_loc=%d, \n",
                           my_verify_callback_results.attestation_scheme,
                           my_verify_callback_results.err_loc);
            switch (my_verify_callback_results.attestation_scheme) {
                case RA_TLS_ATTESTATION_SCHEME_EPID:
                    mbedtls_printf("    epid.ias_enclave_quote_status=%s\n\n",
                                   my_verify_callback_results.epid.ias_enclave_quote_status);
                    break;
                case RA_TLS_ATTESTATION_SCHEME_DCAP:
                    mbedtls_printf("    dcap.func_verify_quote_result=0x%x, "
                                   "dcap.quote_verification_result=0x%x\n\n",
                                   my_verify_callback_results.dcap.func_verify_quote_result,
                                   my_verify_callback_results.dcap.quote_verification_result);
                    break;
                default:
                    mbedtls_printf("  ! unknown attestation scheme!\n\n");
                    break;
            }

            goto exit;
        }
    }

    mbedtls_printf(" ok\n");

    mbedtls_printf("  . Verifying peer X.509 certificate...");

    flags = mbedtls_ssl_get_verify_result(&ssl);
    if (flags != 0) {
        char vrfy_buf[512];
        mbedtls_printf(" failed\n");
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        mbedtls_printf("%s\n", vrfy_buf);

        /* verification failed for whatever reason, fail loudly */
        goto exit;
    } else {
        mbedtls_printf(" ok\n");
    }

    mbedtls_printf("  > Write to server:");
    fflush(stdout);

    len = sprintf((char*)buf, GET_REQUEST);

    while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            goto exit;
        }
    }

    len = ret;
    mbedtls_printf(" %lu bytes written\n\n%s", len, (char*)buf);

    mbedtls_printf("  < Read from server:");
    fflush(stdout);

    do {
        len = sizeof(buf) - 1;
        memset(buf, 0, sizeof(buf));
        ret = mbedtls_ssl_read(&ssl, buf, len);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;

        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            break;

        if (ret < 0) {
            mbedtls_printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
            break;
        }

        if (ret == 0) {
            mbedtls_printf("\n\nEOF\n\n");
            break;
        }

        len = ret;
        mbedtls_printf(" %lu bytes read\n\n%s", len, (char*)buf);
    } while (1);

    mbedtls_ssl_close_notify(&ssl);
    exit_code = MBEDTLS_EXIT_SUCCESS;
exit:
#ifdef MBEDTLS_ERROR_C
    if (exit_code != MBEDTLS_EXIT_SUCCESS) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf);
    }
#endif
    if (ra_tls_verify_lib)
        dlclose(ra_tls_verify_lib);

    mbedtls_net_free(&server_fd);

    mbedtls_x509_crt_free(&cacert);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);

    return exit_code;
}

/* RA-TLS: mbedTLS-specific callback to verify the x509 certificate */
int Attestator::my_verify_callback(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags) {
    if (depth != 0) {
        /* the cert chain in RA-TLS consists of single self-signed cert, so we expect depth 0 */
        return MBEDTLS_ERR_X509_INVALID_FORMAT;
    }
    if (flags) {
        /* mbedTLS sets flags to signal that the cert is not to be trusted (e.g., it is not
         * correctly signed by a trusted CA; since RA-TLS uses self-signed certs, we don't care
         * what mbedTLS thinks and ignore internal cert verification logic of mbedTLS */
        *flags = 0;
    }
    return ra_tls_verify_callback_extended_der_f(crt->raw.p, crt->raw.len, (struct ra_tls_verify_callback_results*)data);
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

bool Attestator::getenv_client_inside_sgx() {
    char* str = getenv("RA_TLS_CLIENT_INSIDE_SGX");
    if (!str)
        return false;

    return !strcmp(str, "1") || !strcmp(str, "true") || !strcmp(str, "TRUE");
}


int Attestator::parse_hex(const char* hex, void* buffer, size_t buffer_size) {
    if (strlen(hex) != buffer_size * 2)
        return -1;

    for (size_t i = 0; i < buffer_size; i++) {
        if (!isxdigit(hex[i * 2]) || !isxdigit(hex[i * 2 + 1]))
            return -1;
        sscanf(hex + i * 2, "%02hhx", &((uint8_t*)buffer)[i]);
    }
    return 0;
}
