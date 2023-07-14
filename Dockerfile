######### BUILD #########
FROM ubuntu:20.04 AS build

RUN apt update && \
    apt install -y make \
    libyaml-cpp-dev \
    g++

WORKDIR /

COPY src/ ./src/
COPY lib/ /usr/lib/
COPY include/ /usr/include
COPY buildenv.mk .
COPY Makefile .

RUN make

######### RUN #########

FROM ubuntu:20.04 AS run

RUN mkdir app
RUN cd app

#COPY --chown=chhan:chhan --from=build ./hello/Controller ./app/
COPY --from=build /controller ./
COPY --chown=chhan:chhan --from=build /lib/libyaml-cpp.so.0.6 /lib

ENTRYPOINT [ "./controller" ]
