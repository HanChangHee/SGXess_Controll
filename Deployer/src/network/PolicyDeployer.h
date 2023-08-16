/*
 * PolicyDeployer.h
 *
 *  Created on: 2023. 4. 14.
 *      Author: chhan
 */

#ifndef SRC_NETWORK_POLICYDEPLOYER_H_
#define SRC_NETWORK_POLICYDEPLOYER_H_

#include <string>
#include "Attestator.h"

class PolicyDeployer {
public:
	PolicyDeployer();
	virtual ~PolicyDeployer();

	bool deployPolicy(std::string &policy, std::string &IP, unsigned short port);

private:
	bool setConnection(std::string IP, unsigned short port);
	void destroyConnection();

private:
	unsigned int m_conn;
	Attestator m_attestator;
};

#endif /* SRC_NETWORK_POLICYDEPLOYER_H_ */
