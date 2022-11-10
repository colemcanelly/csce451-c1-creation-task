CXX=g++
CXXFLAGS=-std=c++17 -g -pedantic -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer
LDLIBS= # -I./include -L ./lib -l:libncurses.a -lreadline -lhistory 


SRCS=shell.cpp
DEPS=Command.cpp Tokenizer.cpp # Custom.cpp
BINS=$(SRCS:%.cpp=%.exe)
OBJS=$(DEPS:%.cpp=%.o)


all: clean $(BINS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.exe: %.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(patsubst %.exe,%,$@) $^ $(LDLIBS)


.PHONY: clean test

clean:
	rm -f shell a b test.txt output.txt out.trace ./test-files/cmd.txt

test: all
	chmod u+x pa2-tests.sh
	./pa2-tests.sh $(filter-out $@,$(MAKECMDGOALS))

%:		# Do nothing if an arg is not a `Make Goal`
	@:
