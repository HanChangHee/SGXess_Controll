/*
 * ACValidator.cpp
 *
 *  Created on: 2023. 3. 29.
 *      Author: chhan
 */
#include <string>
#include "ACValidator.h"


ACValidator::ACValidator() {
}

ACValidator::~ACValidator() {
}

bool ACValidator::validate(int clientSock) {
	std::string data;

	// recv request
	if ( false == SocketUtil::recv(clientSock, data) ) {
		printf("[ACValidator] recv request failed\n");
		return false;
	}

	// todo: parse data
	// todo: validate request


	// send response
	if ( false == SocketUtil::send(clientSock, data) ) {
		printf("[ACValidator] send response failed\n");
		return false;
	}

	return false;
}
