
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/error.h"
#include "RATLS.h"
#include "TLSServer.h"

TLSServer::TLSServer() {
	m_RATLS = false;
}

TLSServer::~TLSServer() {
}


void TLSServer::init() {
	mbedtls_net_init(&m_client);
	mbedtls_net_init(&m_server);
	mbedtls_ssl_init(&m_ssl);
	mbedtls_ssl_config_init(&m_conf);
	mbedtls_x509_crt_init(&m_srvcert);
	mbedtls_pk_init(&m_pkey);
	mbedtls_entropy_init(&m_entropy);
	mbedtls_ctr_drbg_init(&m_ctr_drbg);
}

void TLSServer::destroy() {
	mbedtls_net_free(&m_client);
	mbedtls_net_free(&m_server);
	mbedtls_ssl_free(&m_ssl);
	mbedtls_ssl_config_free(&m_conf);
	mbedtls_x509_crt_free(&m_srvcert);
	mbedtls_pk_free(&m_pkey);
	mbedtls_entropy_free(&m_entropy);
	mbedtls_ctr_drbg_free(&m_ctr_drbg);
}

void TLSServer::resetClient() {
	mbedtls_net_free(&m_client);
	mbedtls_ssl_session_reset(&m_ssl);
}

void TLSServer::terminate() {
	mbedtls_ssl_close_notify(&m_ssl);
	destroy();

	mbedtls_exit(0);
}

bool TLSServer::acceptClient() {
	int ret = 0;
	size_t client_ip_len = 0;
	unsigned char client_ip[16] = {0, };

	mbedtls_printf("  . Waiting for a remote connection ...");
	fflush(stdout);

	if ((ret = mbedtls_net_accept(&m_server, &m_client, client_ip, sizeof(client_ip), &client_ip_len)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_net_accept returned %d\n\n", ret);
		return false;
	}

	mbedtls_ssl_set_bio(&m_ssl, &m_client, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);
	mbedtls_printf(" ok\n");

	// Handshake
	mbedtls_printf("  . Performing the DTLS handshake...");
	fflush(stdout);

	do {
		ret = mbedtls_ssl_handshake(&m_ssl);
	} while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	if (ret != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", (unsigned int) -ret);

        	char error_buf[100];
        	mbedtls_strerror(ret, error_buf, 100);
        	mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf);

		return false;
	}

	mbedtls_printf(" ok\n");

	return true;
}

bool TLSServer::recvMsg(std::string &msg) {
	char buf[1024] = {0, };
	int len = sizeof(buf) - 1;
	int ret = 0;

	mbedtls_printf("  < Read from server:");
	fflush(stdout);

	do {
		ret = mbedtls_ssl_read(&m_ssl, (unsigned char *)buf, len);
	} while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	if (ret <= 0) {
		switch (ret) {
		case MBEDTLS_ERR_SSL_TIMEOUT:
			mbedtls_printf(" timeout\n\n");
			return true;
			break;

		case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
			mbedtls_printf(" connection was closed gracefully\n");
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
	msg.append(buf);
	return true;
}

bool TLSServer::sendMsg(std::string msg) {
	int ret = 0;

	//mbedtls_printf("  > Write to server:");
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

bool TLSServer::setListener(std::string port, bool bRATLS) {
	m_RATLS = bRATLS;
	init();

	void* attestLib = NULL;
	int (*createKey)(uint8_t** der_key, size_t* der_key_size, uint8_t** der_crt,
                                           size_t* der_crt_size);
	const char *pers = "dtls_server";
	int ret = 0;

	do {
		if ( true == m_RATLS ) {
			std::string attestType;
			if ( false == RATLS::getAttestType(attestType) ) {
				ret = 1;
				break;
			}
			if ( false == RATLS::getAttestLib(&attestLib, &createKey) ) {
				ret = 1;
				break;
			}
		}

		mbedtls_printf("  . Seeding the random number generator...");
		fflush(stdout);

		if (0 != (ret = mbedtls_ctr_drbg_seed(&m_ctr_drbg, mbedtls_entropy_func, &m_entropy,
						(const unsigned char*) pers, strlen(pers)))) {
			mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
			break;
		}

		mbedtls_printf(" ok\n");

		// load certificate
		if ( true == m_RATLS ) {
			if ( false == RATLS::setCertificate(m_srvcert, m_pkey, m_ctr_drbg, createKey) ) {
				ret = 1;
				break;
			}
		}
		else {
			if ( false == loadCertificate() ) {
				ret = 1;
				break;
			}
		}

		// Setup the listening UDP socket
		mbedtls_printf("  . Bind on udp/*/%s ...", port.c_str());
		fflush(stdout);

		if ((ret = mbedtls_net_bind(&m_server, NULL, port.c_str(), MBEDTLS_NET_PROTO_TCP)) != 0) {
			mbedtls_printf(" failed\n  ! mbedtls_net_bind returned %d\n\n", ret);
			break;
		}

		mbedtls_printf(" ok\n");

		// set ssl configuration
		if ( false == setSSLConfig() ) {
			ret = 1;
			return false;
		}
	} while(false);

	if ( 0 != ret ) {
		return false;
	}

	return true;
}

bool TLSServer::setSSLConfig() {
	int ret = 0;

	mbedtls_printf("  . Setting up the TLS data...");
	fflush(stdout);

	if ((ret = mbedtls_ssl_config_defaults(&m_conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
		return false;
	}

	mbedtls_ssl_conf_rng(&m_conf, mbedtls_ctr_drbg_random, &m_ctr_drbg);
	//mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);
	mbedtls_ssl_conf_read_timeout(&m_conf, 10000);

	if ( false == m_RATLS ) {
		mbedtls_ssl_conf_ca_chain(&m_conf, m_srvcert.next, NULL);
	}

	if ((ret = mbedtls_ssl_conf_own_cert(&m_conf, &m_srvcert, &m_pkey)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
		return false;
	}

	if ((ret = mbedtls_ssl_setup(&m_ssl, &m_conf)) != 0) {
		mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
		return false;
	}

	//mbedtls_ssl_set_timer_cb(&m_ssl, &m_timer, mbedtls_timing_set_delay, mbedtls_timing_get_delay);
	mbedtls_printf(" ok\n");

	return true;
}

bool TLSServer::loadCertificate() {
	int ret = 0;

	mbedtls_printf("  . Loading the CA root certificate ...");
	fflush(stdout);

	ret = mbedtls_x509_crt_parse_file(&m_srvcert, SRV_CRT_PATH);

	if (ret < 0) {
		mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", (unsigned int) -ret);
		fflush(stdout);
		return false;
	}
	else {
		mbedtls_printf(" successs to get %sn", SRV_CRT_PATH);
		fflush(stdout);
	}

	ret = mbedtls_pk_parse_keyfile(&m_pkey, SRV_KEY_PATH, NULL, mbedtls_ctr_drbg_random, &m_ctr_drbg);

    if (ret != 0) {
        mbedtls_printf(" failed\n  !  mbedtls_pk_parse_key returned %d\n\n", ret);
        fflush(stdout);
        return false;
    }
    else {
    	mbedtls_printf(" successs to get %s\n", SRV_KEY_PATH);
    	fflush(stdout);
    }

	mbedtls_printf(" ok (%d skipped)\n", ret);
	fflush(stdout);
	return true;
}
