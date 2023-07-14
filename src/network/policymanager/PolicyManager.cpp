/*
 * PolicyManager.cpp
 *
 *  Created on: 2023. 3. 29.
 *      Author: chhan
 */

#include "SocketUtil.h"
#include "PolicyManager.h"

PolicyManager::PolicyManager() {
}

PolicyManager::~PolicyManager() {
}

bool PolicyManager::manage(int clientSock) {
	std::string data;

	// recv request
	if ( false == SocketUtil::recv(clientSock, data) ) {
		printf("[PolicyManager] recv request failed\n");
		return false;
	}

	printf("received data: %s\n", data.c_str());
	// todo: parse data
	// todo: manage request

	// send response
	if ( false == SocketUtil::send(clientSock, data) ) {
		printf("[PolicyManager] send response failed\n");
		return false;
	}

	return false;

}
