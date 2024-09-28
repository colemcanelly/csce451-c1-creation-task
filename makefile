NAME=$(shell basename $(CURDIR))

BINDIR=bin
OBJDIR=obj
DEPDIR=dep

SRCDIR=src
INCDIR=include

BIN=$(BINDIR)/$(NAME).out

# Compiler settings
CPP=g++
DEPFLAGS=-MP -MMD
OPT=-O0
DEBUGFLAGS=-fno-stack-protector -Wstringop-overflow=0 -Wformat-overflow=0
CPPFLAGS=-std=c++17 -m32 $(DEBUGFLAGS) -I$(INCDIR) $(OPT) $(DEPFLAGS)
LDLIBS= -lreadline

# File list generation
SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS=$(patsubst $(SRCDIR)/%.cpp,$(DEPDIR)/%.d,$(SRCS))


all: $(BIN)

# Link the final binary
$(BIN): $(OBJS) | $(BINDIR)
	@ $(CPP) $(CPPFLAGS) -o $@ $^ $(LDLIBS)
	$(info ./$@)

# Compile to objects
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) $(DEPDIR)
	$(info $<...)
	@ $(CPP) $(CPPFLAGS) -MF $(DEPDIR)/$*.d -c -o $@ $<


# Create dirs if they don't exist
$(OBJDIR): ; @mkdir -p $(OBJDIR)
$(BINDIR): ; @mkdir -p $(BINDIR)
$(DEPDIR): ; @mkdir -p $(DEPDIR)

# include the dependencies
-include $(DEPS)

.PHONY: clean test

clean:
	rm -f $(OBJDIR)/*.o $(DEPDIR)/*.d $(BINDIR)/*.out TestWebServer
	rm -f shell a b test.txt output.txt out.trace ./test-files/cmd.txt 

test: all
	chmod u+x pa2-tests.sh
	./pa2-tests.sh $(BIN) $(filter-out $@,$(MAKECMDGOALS))

%:		# Do nothing if an arg is not a `Make Goal`
	@:
