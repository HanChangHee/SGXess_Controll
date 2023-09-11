#ifndef POLICYMANAGER_H_
#define POLICYMANAGER_H_

#include <mutex>
#include <string>
#include <list>

class PolicyManager {
public:
	PolicyManager();
	virtual ~PolicyManager();

	void insertPolicy(std::string &policy);
	void popPolicy(std::string &policy);
	void managePolicy();

private:

private:
	std::list<std::string> m_policyList;
	std::mutex m_mutex;
};

#endif /* POLICYMANAGER_H_ */
