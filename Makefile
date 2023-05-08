CC := g++
SRCDIR := src
BUILDDIR := build
TARGET := Controller

SRCEXT := cpp
HEADEREXT := h
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
HEADERS := $(shell find $(SRCDIR) -type f -name *.$(HEADEREXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall
LINK := -lpthread -lyaml-cpp
INC :=	-I /usr/local/include -L/usr/local/lib \
	-I $(SRCDIR)/network \
	-I $(SRCDIR)/network/accesscontrol \
	-I $(SRCDIR)/network/policymanager \
	-I $(SRCDIR)/policy \
	-I $(SRCDIR)/utils

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $(TARGET) $(LINK)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $< $(LINK)

clean:
	@echo "Cleaning..."
	$(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean

