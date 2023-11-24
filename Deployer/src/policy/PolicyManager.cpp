
#include <stdio.h>
#include <unistd.h>

#include "Policy.h"
#include "PolicyDeployer.h"
#include "PolicyManager.h"

PolicyManager::PolicyManager() {
}

PolicyManager::~PolicyManager() {
}

void PolicyManager::insertPolicy(std::string &policy) {

	m_mutex.lock();

	m_policyList.push_back(policy);

	m_mutex.unlock();
}

void PolicyManager::popPolicy(std::string &policy) {
	m_mutex.lock();

	if ( false == m_policyList.empty() ) {
		policy = m_policyList.front();
		m_policyList.pop_front();
	}

	m_mutex.unlock();
}

void PolicyManager::managePolicy() {

	while(true) {
		std::string policy;
		popPolicy(policy);

		if ( true == policy.empty() ) {
			sleep(1);
			continue;
		}

		// Policy parsedPolicy(policy);

		// todo: analysis policy deploy target
		// todo: deploy policy string

		PolicyDeployer::deployPolicy(policy, "192.168.0.5", "30010");
	}
}

