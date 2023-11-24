

#ifndef PROXYMANAGER_H_
#define PROXYMANAGER_H_

#include <string>
#include <list>
#include <mutex>

#include "TLSClient.h"

class ProxyManager {
public:
	ProxyManager();
	virtual ~ProxyManager();

	void insertClient(std::string client);
	void popClient(std::string &client);

	bool manageClient();

private:
	bool setTLSClient();

private:
	std::list<std::string> m_clientList;
	std::mutex m_mutex;
	TLSClient m_tlsClient;
};

#endif /* PROXYMANAGER_H_ */
