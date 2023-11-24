#ifndef PROXYLISTENER_H_
#define PROXYLISTENER_H_

#include "Listener.h"
#include "SocketUtil.h"
#include "ProxyManager.h"

class ProxyListener {
public:
	ProxyListener();
	virtual ~ProxyListener();

	// init listen socket
	bool init();

	static void *run(void *argv);

	// listen for connection from deployer
	bool runListener(ProxyManager *manager);

private:
	Listener m_server;
};

#endif /* PROXYLISTENER_H_ */
