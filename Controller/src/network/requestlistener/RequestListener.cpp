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
	m_start = 0;
}

RequestListener::~RequestListener() {
}

bool RequestListener::init() {
	m_start = clock();
	double start = (double) m_start;

	bool ret = true;
	if ( 1 == SGX ) {
		ret = m_server.setListener("30011", true);
	}
	else {
		ret = m_server.setListener("30011", false);
	}
	clock_t end = clock();
	printf("time to run request listener: %f sec \n", (double)((double)end - start) / CLOCKS_PER_SEC);

	return ret;
}

void *RequestListener::run(void *argv) {
	if ( NULL == argv ) {
		return 0;
	}

	RequestListener listener;
	PolicyManager *manager = (PolicyManager *)argv;

	if ( false == listener.init() ) {
		printf("Request Listener init failed\n");
		return 0;
	}

	printf("Request Listener done\n");
	listener.runListener(manager);

	return 0;
}

bool RequestListener::runListener(PolicyManager *manager) {
	if ( false == m_server.acceptClient() ) {
		printf("Request Listener failed to get client \n");
		return false;
	}

	while(true) {
		// receive and validate data from clients
		std::string req;

		if ( false == m_server.recvMsg(req) ) {
			if ( false == m_server.acceptClient() ) {
				printf("Request Listener failed to get client \n");
				return false;
			}
			continue;
		}
		else if ( true == req.empty() ) {
			continue;
		}

		// Validate the request
		bool vRet = manager->validateRequest(req);
		m_server.sendMsg(vRet ? "Accept":"Deny");

		// todo: send result of validation
		//m_server.resetClient();
	}

	return false;
}
