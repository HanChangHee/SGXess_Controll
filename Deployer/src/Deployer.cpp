
#include "PolicyListener.h"
#include "PolicyDeployer.h"
#include "Thread.h"

int main(int argc, char **argv) {
	PolicyDeployer deployer;
	std::string policyData = "\
	apiVersion: security.istio.io/v1beta1\n\
	kind: AuthorizationPolicy\n\
	metadata:\n\
	  name: httpbin\n\
	  namespace: foo\n\
	spec:\n\
	  action: ALLOW\n\
	  rules:\n\
	  - from:\n\
	    - source:\n\
	        principals: [\"cluster.local/ns/default/sa/sleep\"]\n\
	    - source:\n\
	        namespaces: [\"test\"]\n\
	    to:\n\
	    - operation:\n\
	        methods: [\"GET\"]\n\
	        paths: [\"/info*\"]\n\
	    - operation:\n\
	        methods: [\"POST\"]\n\
	        paths: [\"/data\"]\n\
	    when:\n\
	    - key: request.auth.claims[iss]\n\
	      values: [\"https://accounts.google.com\"]";
	std::string ip = "192.168.0.5";

	deployer.deployPolicy(policyData, ip, 30001);

	return 0;
}
