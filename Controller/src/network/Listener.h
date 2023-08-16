/*
 * Listener.h
 *
 *  Created on: 2023. 4. 10.
 *      Author: chhan
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include "Attestator.h"

class Listener {
public:
	Listener();
	virtual ~Listener();

	bool init(unsigned short port);
	int getClientMsgRATLS(std::string &msg);
	int getClientConn();
	void deleteConn();

private:
	bool listenTCP(unsigned short port);
	bool listenRATLS(unsigned short port);

private:
	int m_sock;
	Attestator m_attestator;
};

#endif /* LISTENER_H_ */
