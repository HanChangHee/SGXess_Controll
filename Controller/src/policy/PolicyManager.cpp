
#include <unistd.h>

#include "SocketUtil.h"
#include "PolicyManager.h"

PolicyManager::PolicyManager() {
}

PolicyManager::~PolicyManager() {
}


void PolicyManager::insertPolicy(std::string &policy) {
	m_mutex.lock();

	m_updateList.push_back(policy);

	m_mutex.unlock();
}

void PolicyManager::popPolicy(std::string &policy) {
	m_mutex.lock();

	if ( false == m_updateList.empty() ) {
		policy = m_updateList.front();
		m_updateList.pop_front();
	}

	m_mutex.unlock();
}

void PolicyManager::updatePolicy(Policy &policy) {

}

bool PolicyManager::validateRequest(std::string &req) {
	return true;
}

void PolicyManager::managePolicy() {
	while(true) {
		std::string policy;
		popPolicy(policy);

		if ( true == policy.empty() ) {
			sleep(1);
			continue;
		}

		Policy parsedPolicy(policy);

		// todo: update policy
		updatePolicy(parsedPolicy);
	}
}

