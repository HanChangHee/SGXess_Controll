/*
 * PolicySelector.h
 *
 *  Created on: 2023. 4. 12.
 *      Author: chhan
 */

#ifndef POLICYSELECTOR_H_
#define POLICYSELECTOR_H_

#include <yaml-cpp/yaml.h>
#include <string>
#include <map>

class PolicySelector {
public:

public:
	PolicySelector();
	virtual ~PolicySelector();

	void setRules(const YAML::Node &selector);

private:
	std::map<std::string, std::string> m_matchLabels;
};

#endif /* POLICYSELECTOR_H_ */
