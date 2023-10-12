#include <stdio.h>
#include <string>

#include "Listener.h"
#include "SocketUtil.h"

#include "TLSClient.h"
#include "validator/Validator.h"

int main() {
	Listener server;
	if ( false == server.init(30020) ) {
		printf("Failed to init server\n");
		return 0;
	}

	while(true) {
		int client = server.getClientConn();
		if ( -1 != client ) {
			std::string msg;
			SocketUtil::recv(client, msg);

			printf("[Recv msg]\n%s", msg.c_str());
			server.deleteClientConn(client);

			TLSClient tlsclient;
			if ( false == tlsclient.connect("192.168.0.3", "30011") ) {
				tlsclient.terminate();
				continue;
			}

			tlsclient.sendMsg(msg);
			tlsclient.terminate();
		}
		else {
			printf("Failed to get client conn\n");
		}
	}

	/*
	TLSServer server;

	server.setListener("30020");

	while ( true ) {
		if ( false == server.acceptClient() ) {
			printf("PolicyListener failed to get client\n");
			server.resetClient();
			continue;
		}

		Validator::validate(server.getSSL());

		server.resetClient();
	}
	*/

    return 0;
}
