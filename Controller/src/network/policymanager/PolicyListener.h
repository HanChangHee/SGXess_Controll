/*
 * PolicyListener.h
 *
 * Created on: 2023. 3. 29.
 *
 * listen for the connection request from deployer
 * after make connection, send the socket to manager
 *
 * Author: chhan
 */

#ifndef POLICYLISTENER_H_
#define POLICYLISTENER_H_

#include "Listener.h"

class PolicyListener {
public:
	PolicyListener();
	virtual ~PolicyListener();

	// init listen socket
	bool init();

	static void *run(void *argv);

private:
	// listen for connection from deployer
	bool runListener();

private:
	Listener m_listener;
};

#endif /* POLICYLISTENER_H_ */
