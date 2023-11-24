
#include <unistd.h>
#include <time.h>

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

void PolicyManager::updatePolicy(std::string policy) {
	std::string::size_type pos = policy.find("|");
	std::string IP = policy.substr(0, pos);
	bool ret = policy.substr(pos + 1, std::string::npos).compare("allow") ? false : true;

	//printf("Inserted Policy: %s | %d\n", IP.c_str(), ret);

	mapPolicy.insert(std::pair<std::string, bool>(IP, ret));
}

bool PolicyManager::validateRequest(std::string &IP) {
	//clock_t start = clock();
	std::map<std::string, bool>::iterator itPolicy = mapPolicy.find(IP);

	//clock_t end = clock();

	//printf("Time to validate policy: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);

	if ( mapPolicy.end() == itPolicy ) {
		return false;
	}

	return itPolicy->second;
}

void PolicyManager::managePolicy() {
	// insert demo policy
	for ( int i = 0; i < 255; i++ ) {
		for ( int j = 0; j < 255; j++ ) {
			for ( int k = 0; k < 100; k++ ) {
				std::string ip = "192.";
				std::string policy = ip;
				policy.append(std::to_string(i)).append(".");
				policy.append(std::to_string(j)).append(".");
				policy.append(std::to_string(k)).append("|deny");
				updatePolicy(policy);
			}
		}
	}

	while(true) {
		std::string policy;
		popPolicy(policy);

		if ( true == policy.empty() ) {
			sleep(1);
			continue;
		}

		//Policy parsedPolicy(policy);

		// todo: update policy
		//updatePolicy(parsedPolicy);
	}
}

