/*
 * PolicyListener.cpp
 *
 *  Created on: 2023. 4. 14.
 *      Author: chhan
 */

#include <stdio.h>

#include "PolicyDeployer.h"
#include "PolicyListener.h"

PolicyListener::PolicyListener() {
	// TODO Auto-generated constructor stub

}

PolicyListener::~PolicyListener() {
	// TODO Auto-generated destructor stub
}


bool PolicyListener::init() {
	return m_listener.init(30000);
}

void *PolicyListener::run(void *argv) {
	PolicyListener listener;
	if ( false == listener.init() ) {
		printf("ACListener listener init failed\n");
		return 0;
	}

	printf("ACListener listener done\n");
	listener.runListener();

	return 0;
}

bool PolicyListener::runListener() {
	while(true) {
		int clientSock = m_listener.getClientConn();
		if ( -1 == clientSock ) {
			printf("ACListener listener get client failed\n");
			break;
		}

		m_listener.deleteClientConn(clientSock);
	}

	return false;
}
