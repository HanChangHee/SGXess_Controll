/*
 * PolicyDeployer.cpp
 *
 *  Created on: 2023. 4. 14.
 *      Author: chhan
 */

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include "SocketUtil.h"
#include "PolicyDeployer.h"

PolicyDeployer::PolicyDeployer() {
	m_conn = 0;
}

PolicyDeployer::~PolicyDeployer() {
	destroyConnection();
}

bool PolicyDeployer::deployPolicy(std::string &policy, std::string &IP, unsigned short port) {
//	if ( false == setConnection(IP, port) ) {
//		printf("Failed to set connection\n");
//		return false;
//	}
//
//	if ( false == SocketUtil::send(m_conn, policy) ) {
//		printf("Failed to send policy\n%s\n", policy.c_str());
//		return false;
//	}

	m_attestator.sendMsg(policy, IP, port);


	return true;
}

bool PolicyDeployer::setConnection(std::string IP, unsigned short port) {
	destroyConnection();

	m_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP.c_str());
	server_addr.sin_port = htons(port);

	if ( 0 != connect(m_conn, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		printf("Failed to connect\n");
		return false;
	}

	return true;
}

void PolicyDeployer::destroyConnection() {
	if ( 0 != m_conn ) {
		close(m_conn);
		m_conn = 0;
	}
}
