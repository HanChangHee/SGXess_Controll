/*
 * ACListener.cpp
 *
 *  Created on: 2023. 3. 29.
 *      Author: chhan
 */

#include <stdio.h>
#include "ACValidator.h"
#include "ACListener.h"

ACListener::ACListener() {
}

ACListener::~ACListener() {
}

bool ACListener::init() {
	return m_listener.init(30000);
}

void *ACListener::run(void *argv) {
	ACListener listener;
	if ( false == listener.init() ) {
		printf("ACListener listener init failed\n");
		return 0;
	}

	printf("ACListener listener done\n");
	listener.runListener();

	return 0;
}

bool ACListener::runListener() {
	while(true) {
		int clientSock = m_listener.getClientConn();
		if ( -1 == clientSock ) {
			printf("ACListener listener get client failed\n");
			break;
		}

		// todo: receive and validate data from clients
		ACValidator::validate(clientSock);

		//m_listener.deleteClientConn(clientSock);
	}

	return false;
}
