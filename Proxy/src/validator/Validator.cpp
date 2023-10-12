/*
 * Requester.cpp
 *
 *  Created on: 2023. 9. 10.
 *      Author: chhan
 */

#include "Validator.h"
#include "Thread.h"
#include "TLSClient.h"
#include "TLSServer.h"

Validator::Validator() {
}

Validator::~Validator() {

}

void Validator::validate(mbedtls_ssl_context ssl) {
	Thread::runThread(Validator::run, (void *)&ssl);
}

void *Validator::run(void *argv) {
	if ( NULL == argv ) {
		return 0;
	}

	mbedtls_ssl_context ssl = *(mbedtls_ssl_context *)argv;

	// get connection request from client
	std::string req;
	TLSServer::recvMsg(req, ssl);
	printf("[received request]\n%s\n", req.c_str());

	// request validate to Controller
	bool ret = Validator::validateRequest(req);

	// send response to client
	if ( true == ret ) {
		std::string res = "O";
		TLSServer::sendMsg(res, ssl);
	}
	else {
		std::string res = "X";
		TLSServer::sendMsg(res, ssl);
	}

	mbedtls_ssl_free(&ssl);

	return 0;
}

bool Validator::validateRequest(std::string &req) {
	TLSClient client;
	if ( false == client.connect(CONTROLLER_IP, CONTROLLER_PORT )) {
		return false;
	}

	if ( false == client.sendMsg(req) ) {
		return false;
	}

	std::string res;
	if ( false == client.recvMsg(res) ) {
		return false;
	}

	client.terminate();

	return true;
}
