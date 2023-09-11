
#include "TLSClient.h"

#include <string.h>

#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
#include "mbedtls/x509_crt.h"

TLSClient::TLSClient() {
}

TLSClient::~TLSClient() {
}

void TLSClient::init() {
    mbedtls_net_init(&m_server);
    mbedtls_ssl_init(&m_ssl);
    mbedtls_ssl_config_init(&m_conf);
    mbedtls_x509_crt_init(&m_cacert);
    mbedtls_ctr_drbg_init(&m_ctr_drbg);
    mbedtls_entropy_init(&m_entropy);
}

void TLSClient::destroy() {
    mbedtls_net_free(&m_server);
    mbedtls_x509_crt_free(&m_cacert);
    mbedtls_ssl_free(&m_ssl);
    mbedtls_ssl_config_free(&m_conf);
    mbedtls_ctr_drbg_free(&m_ctr_drbg);
    mbedtls_entropy_free(&m_entropy);
}

bool TLSClient::connect(std::string IP, std::string port) {
	int ret;
	const char *pers = "dtls_client";

	// 0. Initialize the RNG and the session data
	init();

	do {
		printf("\n  . Seeding the random number generator...");
		fflush(stdout);

		if (0 != (ret = mbedtls_ctr_drbg_seed(&m_ctr_drbg, mbedtls_entropy_func, &m_entropy,
						(const unsigned char*) pers, strlen(pers)))) {
			printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
			break;
		}

		printf(" ok\n");

		// load certificate
		if ( false == loadCertificate() ) {
			ret = 1;
			break;
		}

		// connect
		printf("  . Connecting to udp/%s/%s...", IP.c_str(), port.c_str());
		fflush(stdout);

		if (0 != (ret = mbedtls_net_connect(&m_server, IP.c_str(),
						port.c_str(), MBEDTLS_NET_PROTO_UDP))) {
			printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
			break;
		}

		printf(" ok\n");

		// set ssl config
		if ( false == setSSLConfig(IP) ) {
			ret = 1;
			break;
		}

		// handshake
		printf("  . Performing the DTLS handshake...");
		fflush(stdout);

		do {
			ret = mbedtls_ssl_handshake(&m_ssl);
		} while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

		if (ret != 0) {
			printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", (unsigned int) -ret);
			break;
		}

		printf(" ok\n");

		// verify certificate
		if ( false == verifyCertificate() ) {
			ret = 1;
			break;
		}
	} while (false);

	if (0 != ret) {
		destroy();
		return false;
	} else {
		return true;
	}
}

void TLSClient::terminate() {
	mbedtls_ssl_close_notify(&m_ssl);
	destroy();

	mbedtls_exit(0);
}

bool TLSClient::sendMsg(std::string &msg) {
	int ret = 0;

	printf("  > Write to server:");
	fflush(stdout);

	do {
		ret = mbedtls_ssl_write(&m_ssl, (const unsigned char *)msg.c_str(), msg.length());
	} while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	if (ret < 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
		return false;
	}

	mbedtls_printf(" %d bytes written\n\n%s\n\n", ret, msg.c_str());
	return true;
}

bool TLSClient::recvMsg(std::string &msg) {
	unsigned char buf[1024] = {0, };
	int len = sizeof(buf) - 1;
	int ret = 0;

	printf("  < Read from server:");
	fflush(stdout);

	do {
		ret = mbedtls_ssl_read(&m_ssl, buf, len);
	} while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	if (ret <= 0) {
		switch (ret) {
		case MBEDTLS_ERR_SSL_TIMEOUT:
			printf(" timeout\n\n");
			return false;
			break;

		case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
			printf(" connection was closed gracefully\n");
			ret = 0;
			break;

		default:
			mbedtls_printf(" mbedtls_ssl_read returned -0x%x\n\n", (unsigned int) -ret);
			return false;
			break;
		}
	}

	len = ret;
	mbedtls_printf(" %d bytes read\n\n%s\n\n", len, buf);
	return true;
}

bool TLSClient::verifyCertificate() {
	uint32_t flags;
	printf("  . Verifying peer X.509 certificate...");

	/* In real life, we would have used MBEDTLS_SSL_VERIFY_REQUIRED so that the
	 * handshake would not succeed if the peer's cert is bad.  Even if we used
	 * MBEDTLS_SSL_VERIFY_OPTIONAL, we would bail out here if ret != 0 */
	if ((flags = mbedtls_ssl_get_verify_result(&m_ssl)) != 0) {
		printf(" failed\n");
		// fixme
		char vrfy_buf[512] = {0, };
		mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
		printf("%s\n", vrfy_buf);
		return true;
	}

	printf(" ok\n");
	return true;
}

bool TLSClient::loadCertificate() {
	int ret = 0;

    printf("  . Loading the CA root certificate ...");
    fflush(stdout);

    ret = mbedtls_x509_crt_parse_file(&m_cacert, CA_CRT_PATH);

    // todo: add remote attestation tls certificateion

    if (ret < 0) {
        printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", (unsigned int) -ret);
        return false;
    }

    printf(" ok (%d skipped)\n", ret);
	return true;
}

bool TLSClient::setSSLConfig(std::string &IP) {
	int ret = 0;

	// setup ssl configuration
	printf("  . Setting up the DTLS structure...");
	fflush(stdout);

	if ((ret = mbedtls_ssl_config_defaults(&m_conf, 	MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_DATAGRAM,
			MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
		printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
		return false;
	}

	/* OPTIONAL is usually a bad choice for security, but makes interop easier
	 * in this simplified example, in which the ca chain is hardcoded.
	 * Production code should set a proper ca chain and use REQUIRED. */
	mbedtls_ssl_conf_authmode(&m_conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
	mbedtls_ssl_conf_ca_chain(&m_conf, &m_cacert, NULL);
	mbedtls_ssl_conf_rng(&m_conf, mbedtls_ctr_drbg_random, &m_ctr_drbg);
	//mbedtls_ssl_conf_dbg(&m_conf, my_debug, stdout);
	mbedtls_ssl_conf_read_timeout(&m_conf, READ_TIMEOUT_MS);

	if ((ret = mbedtls_ssl_setup(&m_ssl, &m_conf)) != 0) {
		printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
		return false;
	}

	if ((ret = mbedtls_ssl_set_hostname(&m_ssl, IP.c_str())) != 0) {
		printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
		return false;
	}

	mbedtls_ssl_set_bio(&m_ssl, &m_server, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);
	mbedtls_ssl_set_timer_cb(&m_ssl, &m_timer, mbedtls_timing_set_delay, mbedtls_timing_get_delay);

	printf(" ok\n");

	return true;
}

