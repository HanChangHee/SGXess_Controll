include buildenv.mk

CC := g++ -std=c++17
SRCDIR := src
BUILDDIR := build
TARGET := controller
CFLAGS := -Wall -fpermissive

.PHONY: all
        all: controller controller.manifest 
ifeq ($(SGX),1)
        all: controller.manifest.sgx controller.sig
endif

INSTALL_DIR := $(shell pwd)
ARCH_LIBDIR := /lib/$(shell $(CC) -dumpmachine)

SRCEXT := cpp
HEADEREXT := h
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
HEADERS := $(shell find $(SRCDIR) -type f -name *.$(HEADEREXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

LINK := -lpthread -lyaml-cpp -l$(URTS_LIB_NAME) -ldl -lmbedcrypto_gramine -lmbedtls_gramine -lmbedx509_gramine -lcjson

LIB :=  -L/usr/local/lib \
	-L$(THIS_DIR)/lib \
	-L$(SGXSDK)/sdk_libs \
	-L$(SGX_LIBRARY_PATH)
	
INC :=	-I /usr/local/include \
	-I /usr/include/gramine \
	-I $(SGXSDK)/include \
	-I $(SRCDIR)/network \
	-I $(SRCDIR)/network/requestlistener \
	-I $(SRCDIR)/network/policylistener \
	-I $(SRCDIR)/network/tls \
	-I $(SRCDIR)/policy \
	-I $(SRCDIR)/utils \
	-I $(SRCDIR)/attestation

ifeq ($(DEBUG),1)
GRAMINE_LOG_LEVEL = debug
CFLAGS += -g
else
GRAMINE_LOG_LEVEL = error
CFLAGS += -O3
endif

############# BUILD CONTROLLER ################

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $(TARGET) $(LIB) $(LINK)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $< $(LINK)

clean:
	@echo "Cleaning..."
	$(RM) -r $(BUILDDIR) $(TARGET) *.token *.sig *.manifest.sgx *.manifest

############ GENERATE SSL CERTIFICATE #############
ssl/server.crt: ssl/ca_config.conf
	openssl genrsa -out ssl/ca.key 2048
	openssl req -x509 -new -nodes -key ssl/ca.key -sha256 -days 1024 -out ssl/ca.crt -config ssl/ca_config.conf
	openssl genrsa -out ssl/server.key 2048
	openssl req -new -key ssl/server.key -out ssl/server.csr -config ssl/ca_config.conf
	openssl x509 -req -days 360 -in ssl/server.csr -CA ssl/ca.crt -CAkey ssl/ca.key -CAcreateserial -out ssl/server.crt

############ SET GRAMINE CONFIGURATION #############
RA_TYPE ?= dcap
ISVPRODID ?= 0

controller.manifest: controller.manifest.template
	gramine-manifest \
		-Dentrypoint=$$(command -v gramine-ratls) \
		-Dlog_level=$(GRAMINE_LOG_LEVEL) \
		-Darch_libdir=$(ARCH_LIBDIR) \
		-Dra_type=$(RA_TYPE) \
		-Disvprodid=$(ISVPRODID) \
		-Dinstall_dir=$(INSTALL_DIR) \
		$< > $@

controller.sig controller.manifest.sgx: sgx_sign
        @:

.INTERMEDIATE: sgx_sign
sgx_sign: controller.manifest controller
	gramine-sgx-sign \
		--key ${INSTALL_DIR}/enclave-key.pem \
		--manifest $< \
		--output $<.sgx

ifeq ($(SGX),)
GRAMINE = gramine-direct
else
GRAMINE = gramine-sgx
endif




