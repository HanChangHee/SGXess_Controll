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

THIS_DIR := $(shell pwd)
INSTALL_DIR ?= $(THIS_DIR)
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
	-I $(SRCDIR)/network/accesscontrol \
	-I $(SRCDIR)/network/policymanager \
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

############ SET GRAMINE CONFIGURATION #############
RA_TYPE ?= epid
ISVPRODID ?= 0

controller.manifest: controller.manifest.template
	gramine-manifest \
		-Dlog_level=$(GRAMINE_LOG_LEVEL) \
		-Darch_libdir=$(ARCH_LIBDIR) \
		-Dra_type=$(RA_TYPE) \
		-Disvprodid=$(ISVPRODID) \
		-Dinstall_dir=$(INSTALL_DIR) \
		-Dthis_dir=$(THIS_DIR) \
		$< $@

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




