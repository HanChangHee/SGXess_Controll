/*
 * PolicyManager.h
 *
 * Created on: 2023. 3. 29.
 *
 * do communication with deployer
 * receive policy data from deployer and insert into policy list
 *
 * Author: chhan
 */

#ifndef POLICYMANAGER_H_
#define POLICYMANAGER_H_

#include <string>

class PolicyManager {
public:
	PolicyManager();
	virtual ~PolicyManager();

	static bool manage(int clientSock);
};

#endif /* POLICYMANAGER_H_ */
