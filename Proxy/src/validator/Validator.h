/*
 * Requester.h
 *
 *  Created on: 2023. 9. 10.
 *      Author: chhan
 */

#ifndef REQUESTER_H_
#define REQUESTER_H_

#define CONTROLLER_IP "192.168.0.5"
#define CONTROLLER_PORT "30011"

#include <string>

#include "mbedtls/net_sockets.h"

class Validator {
public:
	Validator();
	virtual ~Validator();

	static void validate(mbedtls_ssl_context ssl);

private:
	static void *run(void *argv);

	static bool validateRequest(std::string &req);
};

#endif /* REQUESTER_H_ */
