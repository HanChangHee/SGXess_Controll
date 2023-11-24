
#include <stdio.h>

#include "SocketUtil.h"
#include "PolicyManager.h"

#include "PolicyListener.h"

PolicyListener::PolicyListener() {
}

PolicyListener::~PolicyListener() {
}

bool PolicyListener::init() {
	bool ret = true;
	clock_t cstart = clock();
	double start = (double) cstart;

	if ( 1 == SGX ) {
		ret =  m_server.setListener("30010", true);
	}
	else {
		ret =  m_server.setListener("30010", false);
	}

	clock_t end = clock();
	printf("time to run policy listener: %f sec \n", (double)((double)end - start) / CLOCKS_PER_SEC);

	return ret;
}

void *PolicyListener::run(void *argv) {
	if ( NULL == argv ) {
		return 0;
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
