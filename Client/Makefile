
CC := g++
SRCDIR := src
BUILDDIR := build
TARGET := client
CFLAGS := -Wall -fpermissive


SRCEXT := cpp
HEADEREXT := h
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
HEADERS := $(shell find $(SRCDIR) -type f -name *.$(HEADEREXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
LINK := -L /usr/local/lib/x86_64-linux-gnu -lpthread -ldl -lmbedcrypto_gramine -lmbedtls_gramine -lmbedx509_gramine

INC := -I /usr/local/include \
       -I /usr/include/gramine \
       -I $(SRCDIR)/network
       
$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $(TARGET) $(LIB) $(LINK)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $< $(LINK)

clean:
	@echo "Cleaning..."
	$(RM) -r $(BUILDDIR) $(TARGET)
