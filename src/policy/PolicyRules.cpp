/*
 * PolicyRules.cpp
 *
 *  Created on: 2023. 4. 12.
 *      Author: chhan
 */

#include "PolicyRules.h"

PolicyRules::PolicyRules() {
}

PolicyRules::~PolicyRules() {
}

bool PolicyRules::setRules(const YAML::Node &rules) {
	const YAML::Node &rule = *rules.begin();

	try {
		// todo: set from
		if ( false == setFrom(rule) ) {
			return false;
		}

		// todo: set to
		setTo(rule);
	} catch (const YAML::ParserException &e) {
		printf("%s\n", e.what());
		return false;
	}

	return true;
}

bool PolicyRules::setFrom(const YAML::Node &rules) {
	const YAML::Node &from = rules["from"];
	if ( YAML::NodeType::Sequence != rules.Type() ) {
		printf("spec > rules > from is not list\n");
		return false;
	}

	for (YAML::const_iterator itRule = rules.begin(); itRule != rules.end(); ++itRule) {
		const YAML::Node& rule = *itRule;
		printf("from_type: %d\n", rule["from"].Type());
		const YAML::Node& from = rule["from"];
		for (YAML::const_iterator itSource = from.begin(); itSource != from.end(); ++itSource) {
			const YAML::Node& source = *itSource;
			printf("source: %s\n", source.begin()->second.begin()->second.begin()->as<std::string>().c_str());
		}
	}

	return false;
}


bool PolicyRules::setTo(const YAML::Node &rules) {
	const YAML::Node &from = rules["to"];

	return false;
}

bool PolicyRules::setWhen(const YAML::Node &rules) {
	// nothing

	return false;
}


