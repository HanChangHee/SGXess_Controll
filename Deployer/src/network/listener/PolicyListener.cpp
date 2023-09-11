#include <stdio.h>

#include "SocketUtil.h"
#include "Policy.h"
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
	if ( NULL == argv ) {
		return -1;
	}

	PolicyManager *manager = (PolicyManager *)argv;
	PolicyListener listener;

	listener.init();
	listener.runListener(manager);

	return 0;
}

bool PolicyListener::runListener(PolicyManager *manager) {
	while(true) {
		int clientSock = m_listener.getClientConn();
		if ( -1 == clientSock ) {
			printf("ACListener listener get client failed\n");
			break;
		}

		// receive policy
		std::string policy;
		SocketUtil::recv(clientSock, policy);

		// save policy
		manager->insertPolicy(policy);
		m_listener.deleteClientConn(clientSock);
	}

	return false;
}
