/*
 * Listener.cpp
 *
 *  Created on: 2023. 4. 10.
 *      Author: chhan
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include "Listener.h"

Listener::Listener() {
	m_sock = 0;
}

Listener::~Listener() {
	deleteConn();
}

bool Listener::init(unsigned short port) {
	if ( 0 != m_sock ) {
		deleteConn();
	}

	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( 0 > m_sock ) {
		printf("Failed to set socket\n");
		return false;
	}

	int ret = 1;
	//setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&ret, sizeof(ret));
	if ( 0 > ret ) {
		printf("Failed to set socket option\n");
		return false;
	}

	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = ::htons(port);

	ret = bind(m_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if ( 0 > ret ) {
		printf("Failed to bind socket %d\n", ret);
		return false;
	}

	ret = listen(m_sock, 5);
	if ( 0 > ret ) {
		printf("Failed to set socket listen\n");
		return false;
	}

	return true;
}

int Listener::getClientConn() {
	sockaddr_in client_addr;
	int clientAddrSize = sizeof(client_addr);
	int clientSock = accept(m_sock, (struct sockaddr *)&client_addr, (socklen_t *)&clientAddrSize);

	if ( 0 > clientSock ) {
		return -1;
	}

	return clientSock;
}

void Listener::deleteConn() {
	if ( 0 != m_sock ) {
		close(m_sock);
		m_sock = 0;
	}
}
