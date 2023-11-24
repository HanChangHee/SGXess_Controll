CC := g++
SRCDIR := src
BUILDDIR := build
TARGET := proxy
CFLAGS := -Wall -fpermissive

.PHONY: all
        all: proxy

THIS_DIR := $(shell pwd)
INSTALL_DIR ?= $(THIS_DIR)
ARCH_LIBDIR := /lib/$(shell $(CC) -dumpmachine)

SRCEXT := cpp
HEADEREXT := h
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
HEADERS := $(shell find $(SRCDIR) -type f -name *.$(HEADEREXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
LINK := -L /usr/local/lib/x86_64-linux-gnu -lpthread -ldl -lmbedcrypto_gramine -lmbedtls_gramine -lmbedx509_gramine

INC := -I /usr/local/include \
	-I /usr/include/gramine \
	-I $(SRCDIR) \
	-I $(SRCDIR)/network \
	-I $(SRCDIR)/validator
 
ifeq ($(DEBUG),1)
CFLAGS += -g
else
CFLAGS += -O3
endif

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $(TARGET) $(LIB) $(LINK)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $< $(LINK)

clean:
	@echo "Cleaning..."
	$(RM) -r $(BUILDDIR) $(TARGET) *.token *.sig *.manifest.sgx *.manifest

