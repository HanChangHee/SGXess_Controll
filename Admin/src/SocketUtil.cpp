/*
 * Receiver.cpp
 *
 *  Created on: 2023. 4. 11.
 *      Author: chhan
 */

#include "SocketUtil.h"

#include <sys/socket.h>


SocketUtil::SocketUtil() {
}

SocketUtil::~SocketUtil() {
}

bool SocketUtil::recv(int targetSock, std::string &data) {
	char buffer[MAX_BUF_LEN] = {0, };

	while(true) {
		int recvLen = ::recv(targetSock, buffer, MAX_BUF_LEN, 0);
		if ( 0 > recvLen ) {
			printf("[SocketUtil] recv failed\n");
			return false;
		}

		data.append(buffer);

		// recv complete
		if ( MAX_BUF_LEN > recvLen ) {
			break;
		}
	}

	return true;
}

bool SocketUtil::send(int targetSock, std::string &data) {
	size_t dataLen = data.size();

	int sendLen = ::send(targetSock, &dataLen, sizeof(dataLen), 0);
	if ( 0 > sendLen ) {
		printf("[SocketUtil] send failed\n");
		return false;
	}

	sendLen = ::send(targetSock, data.c_str(), data.size(), 0);
	if ( 0 > sendLen ) {
		printf("[SocketUtil] send failed\n");
		return false;
	}

	return true;
}
