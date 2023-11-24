#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include "SocketUtil.h"
#include "PolicyDeployer.h"

PolicyDeployer::PolicyDeployer() {
}

PolicyDeployer::~PolicyDeployer() {
}

bool PolicyDeployer::deployPolicy(std::string &policy, std::string IP, unsigned short port) {
	/*
	 * using TCP without encryption
	 */
	PolicyDeployer deployer;
	unsigned int conn = deployer.setConnection(IP, port);
	if ( 0 == conn ) {
		printf("Failed to set connection\n");
		return false;
	}
	printf("connection success. FD:%u\n", conn);
	if ( false == SocketUtil::send(conn, policy) ) {
		printf("Failed to send policy\n%s\n", policy.c_str());
		return false;
	}
	printf("send policy success\n");
	deployer.destroyConnection(conn);

	return true;
}

int PolicyDeployer::setConnection(std::string IP, unsigned short port) {
	unsigned int conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.5");
	server_addr.sin_port = htons(port);

	int ret = connect(conn, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if ( 0 != ret ) {
		printf("Failed to connect\n");
		destroyConnection(conn);
		return 0;
	}

	printf("Connection setted %s:%u, ret: %d, conn: %d\n", IP.c_str(), port, ret, conn);
	return conn;
}

void PolicyDeployer::destroyConnection(unsigned int conn) {
	if ( 0 != conn ) {
		close(conn);
		conn = 0;
	}
}
