# Project-specific settings
PROJECT := SimpleLang

# Identify compiler to use
CXX := c++

# Flags to ALWAYs use
CFLAGS_all := -Wall -Wextra -std=c++20

# Flags based on compilation type.
#   Default flags turn on optimizations
#   Use "make debug" to turn on debugger flag
#   Use "make grumpy" to get extra warnings during compilation
CFLAGS := -O3 -DNDEBUG $(CFLAGS_all)
CFLAGS_debug := -g $(CFLAGS_all)
CFLAGS_grumpy := -pedantic -Wconversion -Weffc++ $(CFLAGS_all)

default: $(PROJECT)
all: $(PROJECT)

debug:	CFLAGS := $(CFLAGS_debug)
debug:	$(PROJECT)

grumpy:	CFLAGS := $(CFLAGS_grumpy)
grumpy:	$(PROJECT)

# List any files here that should trigger full recompilation when they change.
KEY_FILES := 

$(PROJECT):	$(PROJECT).cpp $(KEY_FILES)
	$(CXX) $(CFLAGS) $(PROJECT).cpp -o $(PROJECT)

clean:
	rm -f $(PROJECT) *.o

# Debugging information
print-%: ; @echo '$(subst ','\'',$*=$($*))'
