/*
 * AuthPolicy.h
 *
 * Created on: 2023. 3. 29.
 *
 * treat authorization policy
 * just 2 interface can use from outside, insert and validate
 * make AuthPolicy object as static to used by both policy manager and validator
 *
 * Author: chhan
 */

#ifndef AUTHPOLICY_H_
#define AUTHPOLICY_H_

#include <map>

#include "Policy.h"

class AuthPolicy {
public:
	AuthPolicy();
	virtual ~AuthPolicy();

	// Insert or update policy by received data from deployer
	bool insertPolicy(std::string &sPolicy);

	// validate request from other workloads by policy
	bool validatePolicy();

private:
	// update policy list with inserted policy data
	bool update(Policy &policy);

private:
	std::map<std::string, Policy> mapPolicy;
};

#endif /* AUTHPOLICY_H_ */
