/*
 * PolicyListener.cpp
 *
 *  Created on: 2023. 3. 29.
 *      Author: chhan
 */
#include <stdio.h>
#include "PolicyManager.h"
#include "PolicyListener.h"

PolicyListener::PolicyListener() {
}

PolicyListener::~PolicyListener() {
}

bool PolicyListener::init() {
	return m_listener.init(30001);
}

void *PolicyListener::run(void *argv) {
	PolicyListener listener;
	if ( false == listener.init() ) {
		printf("PolicyListener listener init failed\n");
		return 0;
	}

	printf("PolicyListener listener done\n");
	listener.runListener();

	return 0;
}

bool PolicyListener::runListener() {
	while(true) {
		std::string msg;
		if ( false == m_listener.getClientMsgRATLS(msg) ) {
			break;
		}

		printf("received message from client: %s\n", msg.c_str());

		//m_listener.deleteClientConn(clientSock);
	}

	return false;
}
