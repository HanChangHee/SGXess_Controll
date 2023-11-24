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
#include <mutex>
#include "Policy.h"

class PolicyManager {
public:
	PolicyManager();
	virtual ~PolicyManager();

	// validate request from other workloads by policy
	bool validateRequest(std::string &req);

	void insertPolicy(std::string &policy);

	void managePolicy();

private:
	// update policy list with inserted policy data
	void updatePolicy(std::string policy);
	void popPolicy(std::string &policy);

private:
	std::map<std::string, bool> mapPolicy;

	std::list<std::string> m_updateList;
	std::mutex m_mutex;
};

#endif /* POLICYMANAGER_H_ */
