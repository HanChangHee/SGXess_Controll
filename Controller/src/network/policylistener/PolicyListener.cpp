
#include <stdio.h>

#include "SocketUtil.h"
#include "PolicyManager.h"

#include "PolicyListener.h"

PolicyListener::PolicyListener() {
}

PolicyListener::~PolicyListener() {
}

bool PolicyListener::init() {
	//return m_listener.init(30010);
	return m_server.setListener("30010");
}

void *PolicyListener::run(void *argv) {
	if ( NULL == argv ) {
		return -1;
	}

	PolicyListener listener;
	PolicyManager *manager = (PolicyManager *)argv;

	if ( false == listener.init() ) {
		printf("PolicyListener listener init failed\n");
		return 0;
	}

	printf("PolicyListener listener done\n");
	listener.runListener(manager);

	return 0;
}

bool PolicyListener::runListener(PolicyManager *manager) {
	while ( true ) {
		if ( false == m_server.acceptClient() ) {
			printf("PolicyListener failed to get client\n");
			m_server.resetClient();
			continue;
		}

		// receive policy
		std::string policy;
		m_server.recvMsg(policy);

		printf("[received message]\n%s\n", policy.c_str());

		// todo: send response to policy deployer

		m_server.resetClient();
		printf("received message from client: %s\n", policy.c_str());

		manager->insertPolicy(policy);
	}

	/*
	while(true) {
		int clientSock = m_listener.getClientConn();
		if ( -1 == clientSock ) {
			printf("ACListener listener get client failed\n");
			break;
		}

		// receive policy
		std::string policy;
		SocketUtil::recv(clientSock, policy);

		printf("received message from client: %s\n", policy.c_str());

		m_listener.deleteClientConn(clientSock);

		manager->insertPolicy(policy);
	}
	*/

	return true;
}
