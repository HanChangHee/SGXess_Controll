/*
 * policy.cpp
 *
 *  Created on: 2023. 3. 29.
 *      Author: chhan
 */

#include "Policy.h"

std::string Policy::API_VERSION_KEY = "apiVersion";
std::string Policy::API_VERSION_VALUE = "security.istio.io/v1beta1";
std::string Policy::KIND_KEY = "kind";
std::string Policy::KIND_VALUE = "AuthorizationPolicy";
std::string Policy::METADATA_KEY = "metadata";
std::string Policy::NAME_KEY = "name";
std::string Policy::NAMESPACE_KEY = "namespace";
std::string Policy::SPEC_KEY = "spec";
std::string Policy::ACTION_KEY = "action";
std::string Policy::RULES_KEY = "rules";
std::string Policy::SELECTOR_KEY = "selector";

Policy::Policy() {
	m_action = UNKNOWN;
}

Policy::~Policy() {
}

bool Policy::setPolicy(std::string &policyData) {
	YAML::Node policy;
	try {
		printf("%s\n", policyData.c_str());
		policy = YAML::Load(policyData.c_str());
	} catch (const YAML::ParserException &e) {
		printf("%s\n", e.what());
		return false;
	}

	// check version
	if ( 0 != policy[API_VERSION_KEY].as<std::string>().compare(API_VERSION_VALUE) ) {
		printf("api version is not v1beta1, %s\n", policy[API_VERSION_KEY].as<std::string>().c_str());
		return false;
	}
	// check kind
	if ( 0 != policy[KIND_KEY].as<std::string>().compare(KIND_VALUE) ) {
		printf("kind is not AuthorizationPolicy, %s\n", policy[KIND_KEY].as<std::string>().c_str());
		return false;
	}
	
	// get metadata
	if ( false == setMetadata(policy[METADATA_KEY]) ) {
		return false;
	}

	// get spec
	const YAML::Node &spec = policy[SPEC_KEY];

	// set action
	if ( false == setAction(spec) ) {
		return false;
	}

	// todo: get rules
	m_rules.setRules(spec[RULES_KEY]);

	// todo: get selector
	m_selector.setRules(spec[SELECTOR_KEY]);

	return true;
}

bool Policy::setMetadata(const YAML::Node &metadata) {
	if ( 1 >= metadata.size() ) {
		return false;
	}

	try {
		m_name = metadata[NAME_KEY].as<std::string>();
		m_namespace = metadata[NAMESPACE_KEY].as<std::string>();
	} catch (const YAML::InvalidNode &e) {
		printf("Failed to get metadata. %s\n", e.what());
		return false;
	}

	printf("[TEST] NAME: %s\nNAMESPACE: %s\n", m_name.c_str(), m_namespace.c_str());
	return true;
}

bool Policy::setAction(const YAML::Node &spec) {
	std::string action;

	try {
		action = spec[ACTION_KEY].as<std::string>();
	} catch (const YAML::InvalidNode &e) {
		printf("Failed to get action. %s\n", e.what());
		return false;
	}

	if ( 0 == action.compare("ALLOW") ) {
		m_action = ALLOW;
	}
	else if ( 0 == action.compare("DENY") ) {
		m_action = DENY;
	}
	else if ( 0 == action.compare("AUDIT") ) {
		m_action = AUDIT;
	}
	else {
		m_action = UNKNOWN;
		return false;
	}

	return true;
}


const PolicyRules& Policy::getRules() {
	return m_rules;
}

const PolicySelector& Policy::getSelector() {
	return m_selector;
}

const Policy::E_Action& Policy::getAction() {
	return m_action;
}


