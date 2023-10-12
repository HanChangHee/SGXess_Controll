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
	size_t dataLen = 0;

	int recvLen = ::recv(targetSock, &dataLen, sizeof(dataLen), 0);

	while(dataLen > 0) {
		char buffer[MAX_BUF_LEN] = {0, };
		int recvDataLen = ::recv(targetSock, buffer, dataLen > MAX_BUF_LEN ? MAX_BUF_LEN:dataLen, 0);
		if ( 0 > recvDataLen ) {
			printf("[SocketUtil] recv failed\n");
			return false;
		}
		data.append(buffer);

		dataLen -= recvDataLen;
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
