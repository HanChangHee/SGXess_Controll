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

#include <time.h>

//#include "Listener.h"
#include "TLSServer.h"

class RequestListener {
public:
	RequestListener();
	virtual ~RequestListener();

	bool init();

	static void *run(void *argv);

private:
	bool runListener(PolicyManager *manager);

private:
	TLSServer m_server;
	clock_t m_start;

};

#endif /* ACLISTENER_H_ */
