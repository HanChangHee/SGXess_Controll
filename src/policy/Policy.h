/*
 * policy.h
 *
 * Created on: 2023. 3. 29.
 *
 * parse policy data written in yaml file
 * save policy data as same format with Istio envoy
 *
 * Author: chhan
 */

#ifndef POLICY_H_
#define POLICY_H_

#include <string>

class Policy {
public:
	// init object with empty values
	Policy();

	// init object with inserted values
	Policy(std::string &sPolicy);

	virtual ~Policy();

	// todo: getter
	// todo: setter(if it needed)

private:
	// todo: values as same with Istio Envoy
};

#endif /* POLICY_H_ */
