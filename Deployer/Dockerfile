######### BUILD #########
FROM gramineproject/gramine AS build

RUN apt update && \
    apt install -y make \
    g++ \
    libyaml-cpp-dev \
    net-tools

WORKDIR /

COPY src/ ./src/
COPY lib/ /usr/lib/
#COPY include/ /usr/include
COPY Makefile .
COPY deployer.manifest.template .
COPY enclave-key.pem .

RUN make SGX=1

######### RUN #########

FROM gramineproject/gramine AS run

RUN mkdir app
RUN cd app

COPY --from=build /deployer ./
COPY --from=build /deployer.manifest ./
COPY --from=build /deployer.manifest.sgx ./
COPY --from=build /deployer.sig ./
COPY --from=build /lib/libyaml-cpp.so.0.6 ./lib/

ENTRYPOINT [ "gramine-sgx", "deployer" ]


