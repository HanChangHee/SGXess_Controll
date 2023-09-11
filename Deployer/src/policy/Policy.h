/*
 * policy.h
 *
 * Created on: 2023. 3. 29.
 *
 * parse policy data written in yaml file
 * save policy data as same format with Istio envoy
 *
 * Author: chhan
 */

#ifndef POLICY_H_
#define POLICY_H_

#include <string>

#include "PolicySelector.h"
#include "PolicyRules.h"

class Policy {
public:
	enum E_Action {
		UNKNOWN, ALLOW, DENY, AUDIT,
	};

	// init object with empty values
	Policy();

	// init object with inserted values
	Policy(std::string &policy);

	virtual ~Policy();

	bool setPolicy(std::string &policy);

	// todo: getter
	const PolicyRules& getRules();
	const PolicySelector& getSelector();
	const E_Action& getAction();

private:
	// todo: values as same with Istio Envoy
	bool setMetadata(const YAML::Node &metadata);
	bool setAction(const YAML::Node &action);

private:
	std::string m_name;
	std::string m_namespace;
	E_Action m_action;
	PolicySelector m_selector;
	PolicyRules m_rules;

	static std::string API_VERSION_KEY;
	static std::string API_VERSION_VALUE;

	static std::string KIND_KEY;
	static std::string KIND_VALUE;

	static std::string METADATA_KEY;
	static std::string NAME_KEY;
	static std::string NAMESPACE_KEY;

	static std::string SPEC_KEY;
	static std::string ACTION_KEY;
	static std::string RULES_KEY;
	static std::string SELECTOR_KEY;
};

#endif /* POLICY_H_ */
