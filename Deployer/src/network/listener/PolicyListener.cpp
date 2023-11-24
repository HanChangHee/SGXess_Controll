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
	bool ret = true;
	clock_t cstart = clock();
	double start = (double) cstart;

	ret = m_listener.init(30005);

	clock_t end = clock();

	printf("time to run policy listener: %f sec \n", (double)((double)end - start) / CLOCKS_PER_SEC);
	fflush(stdout);
	return ret;
}

void *PolicyListener::run(void *argv) {
	if ( NULL == argv ) {
		return -1;
	}

	PolicyManager *manager = (PolicyManager *)argv;
	PolicyListener listener;

	if ( false == listener.init() ) {
		printf("Failed to set listening port 30005\n");
		return -1;
	}
	else {
		printf("Listening port 30005 is setted\n");
	}

	listener.runListener(manager);

	return 0;
}

bool PolicyListener::runListener(PolicyManager *manager) {
	while(true) {
		int clientSock = m_listener.getClientConn();
		printf("Accepted some connection\n");
		fflush(stdout);
		if ( -1 == clientSock ) {
			printf("ACListener listener get client failed\n");
			break;
		}

		// receive policy
		std::string policy;
		SocketUtil::recv(clientSock, policy);

		printf("Recv data: %s\n", policy.c_str());

		// save policy
		manager->insertPolicy(policy);
		m_listener.deleteClientConn(clientSock);
	}

	return false;
}
