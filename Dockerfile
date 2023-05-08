######### BUILD #########
FROM ubuntu AS build

RUN apt update && \
    apt install -y make \
    g++

WORKDIR /hello

COPY src/ ./src/
COPY lib/ /usr/lib/
COPY include/ /usr/include
COPY Makefile .

RUN make

######### RUN #########

FROM ubuntu

#COPY --chown=chhan:chhan --from=build ./hello/Controller ./app/
COPY --from=build ./hello/Controller ./app/

ENTRYPOINT [ "./app/Controller" ]


