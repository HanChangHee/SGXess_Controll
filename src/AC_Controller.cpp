#include <unistd.h>

#include "Thread.h"
#include "ACListener.h"
#include "PolicyListener.h"

#include "Policy.h"
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

int main(int argc, char **argv) {

	// 1. run as daemon
//	int ret = fork();
//	if ( getpid() == ret ) {
//		return 0;
//	}
//	else if ( -1 == ret ) {
//		return 0;
//	}

	// 2. init AuthPolicy as static
	// 3. run listeners by master listener, with no thread
	//Thread::runThread(ACListener::run);

	//Thread::runThread(PolicyListener::run);

	Policy policy;
	policy.setPolicy(policyData);

	return 0;
}

