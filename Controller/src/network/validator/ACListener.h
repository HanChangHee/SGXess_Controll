/*
 * ACListener.h
 *
 * Created on: 2023. 3. 29.
 *
 * listen for the connection request from sidecar proxy
 * after make connection, send the socket to validator
 *
 * Author: chhan
 */

#ifndef ACLISTENER_H_
#define ACLISTENER_H_

#include "Listener.h"

class ACListener {
public:
	ACListener();
	virtual ~ACListener();

	bool init();

	static void *run(void *argv);

private:
	bool runListener();

private:
	Listener m_listener;
};

#endif /* ACLISTENER_H_ */
