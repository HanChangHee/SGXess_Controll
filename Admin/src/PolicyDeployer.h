/*
 * PolicyDeployer.h
 *
 *  Created on: 2023. 4. 14.
 *      Author: chhan
 */

#ifndef POLICYDEPLOYER_H_
#define POLICYDEPLOYER_H_

#include <string>

class PolicyDeployer {
public:
	static bool deployPolicy(std::string &policy, std::string IP, unsigned short port);

	int setConnection(std::string IP, unsigned short port);
	void destroyConnection(unsigned int conn);

private:
	PolicyDeployer();
	virtual ~PolicyDeployer();

};

#endif /* POLICYDEPLOYER_H_ */
