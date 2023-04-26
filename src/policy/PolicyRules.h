/*
 * PolicyRules.h
 *
 *  Created on: 2023. 4. 12.
 *      Author: chhan
 */

#ifndef POLICYRULES_H_
#define POLICYRULES_H_

#include <yaml-cpp/yaml.h>
#include <string>
#include <set>
#include <map>

class PolicyRules {
public:
	enum E_SOURCE {
		UNKNOWN_SOURCE, PRINCIPALS, NOT_PRINCIPALS, REQUEST_PRINCIPALS, REQUEST_NOT_PRINCIPALS, NAMESPACES, NOT_NAMESPACES,
		IP_BLOCKS, NOT_IP_BLOCKS, REMOTE_IP_BLOCKS, REMOTE_NOT_IP_BLOCKS
	};

	enum E_OPERATION {
		UNKNOWN_OPERATION, HOSTS, NOT_HOSTS, PORTS, NOT_PORTS, METHODS, NOT_METHODS, PATHS, NOT_PATHS
	};

public:
	PolicyRules();
	virtual ~PolicyRules();

	bool setRules(const YAML::Node &rules);

private:
	bool setFrom(const YAML::Node &rules);
	bool setTo(const YAML::Node &rules);
	bool setWhen(const YAML::Node &rules);

private:
	std::map<E_SOURCE, std::set<std::string> > m_source;
	std::map<E_OPERATION, std::set<std::string> > m_operation;
};

#endif /* POLICYRULES_H_ */
