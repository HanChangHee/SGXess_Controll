/*
 * ProxyListener.cpp
 *
 *  Created on: 2023. 10. 21.
 *      Author: chhan
 */

#include <arpa/inet.h>
#include "ProxyListener.h"

ProxyListener::ProxyListener() {
	// TODO Auto-generated constructor stub

}

ProxyListener::~ProxyListener() {
	// TODO Auto-generated destructor stub
}

bool ProxyListener::init() {
	return m_server.init(30020);
}

void *ProxyListener::run(void *argv) {
	if ( NULL == argv ) {
		return 0;
	}

	ProxyListener listener;
	ProxyManager *manager = (ProxyManager *)argv;

	if ( false == listener.init() ) {
		printf("PolicyListener listener init failed\n");
		return 0;
	}

	printf("PolicyListener listener done\n");
	listener.runListener(manager);

	return 0;
}


bool ProxyListener::runListener(ProxyManager *manager) {
	while ( true ) {
		int clientSock = 0;
		sockaddr_in clientAddr;
		if ( -1 == (clientSock = m_server.getClientConn(clientAddr) ) ) {
			printf("PolicyListener failed to get client\n");
			continue;
		}

		char clientIP[32] = {0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
		printf("%s\n", clientIP);

		manager->insertClient(std::string(clientIP, sizeof(clientIP)));
		m_server.deleteClientConn(clientSock);
	}

	return true;
}
