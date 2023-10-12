/*
 * ACListener.cpp
 *
 *  Created on: 2023. 3. 29.
 *      Author: chhan
 */

#include <stdio.h>

#include "SocketUtil.h"
#include "PolicyManager.h"
#include "RequestListener.h"

RequestListener::RequestListener() {
}

RequestListener::~RequestListener() {
}

bool RequestListener::init() {
	return m_server.setListener("30011", true);
}

void *RequestListener::run(void *argv) {
	if ( NULL == argv ) {
		return 0;
	}

	RequestListener listener;
	PolicyManager *manager = (PolicyManager *)argv;

	if ( false == listener.init() ) {
		printf("ACListener listener init failed\n");
		return 0;
	}

	printf("ACListener listener done\n");
	listener.runListener(manager);

	return 0;
}

bool RequestListener::runListener(PolicyManager *manager) {
	while(true) {
		if ( false == m_server.acceptClient() ) {
			printf("ACListener listener get client failed\n");
			break;
		}

		// receive and validate data from clients
		std::string req;
		m_server.recvMsg(req);

		// Validate the request
		bool vRet = manager->validateRequest(req);

		// todo: send result of validation
		m_server.resetClient();
	}

	return false;
}
