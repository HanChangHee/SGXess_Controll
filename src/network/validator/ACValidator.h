/*
 * ACValidator.h
 *
 * Created on: 2023. 3. 29.
 *
 * do communication with sidecar proxy
 * receive policy data from sidecar proxy and authorize by the policy
 * send the result of authorization to sidecar proxy
 *
 * Author: chhan
 */

#ifndef ACVALIDATOR_H_
#define ACVALIDATOR_H_

#include "SocketUtil.h"

class ACValidator {
public:
	ACValidator();
	virtual ~ACValidator();

	static bool validate(int clientSock);
};

#endif /* ACVALIDATOR_H_ */
