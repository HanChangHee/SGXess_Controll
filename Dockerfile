FROM alpine:3.17.0 AS build

RUN apk update && \
    apk add --no-cache \
    build-base \
    libstdc++ \
    make && 

WORKDIR /home/chhan/eclipse-workspace/AC_Controller

COPY src/ ./src/
COPY Makefile .

RUN make

WORKDIR /home/chhan/eclipse-workspace/AC_Controller

RUN addgroup -S chhan && adduser -S chhan -G chhan
USER chhan

COPY . .

CMD ls -al
#COPY --chown=chhan:chhan ./Controller .

#ENTRYPOINT ["Controller"]

