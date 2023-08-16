/*
 * PolicyListener.h
 *
 *  Created on: 2023. 4. 14.
 *      Author: chhan
 */

#ifndef POLICYLISTENER_H_
#define POLICYLISTENER_H_

#include "Listener.h"

class PolicyListener {
public:
	PolicyListener();
	virtual ~PolicyListener();

	bool init();

	static void *run(void *argv);

private:
	bool runListener();

private:
	Listener m_listener;
};

#endif /* POLICYLISTENER_H_ */
