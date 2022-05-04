
#===========================================================================
#
# Makefile for tsh project --
#
#===========================================================================

BIN     = tsh
ENDLESS = endless
SLOW    = slow

CPP      = g++
CPPFLAGS = -D_GNU_SOURCE -Wall -g
LNFLAGS  = -Wall


CPPFILES := $(wildcard *.cpp)
OBJS := $(CPPFILES:.cpp=.o)
HEADERS := $(wildcard *.h)

%.o: %.cpp
	@echo "Compiling $<..."
	@$(CPP) $(CPPFLAGS) -c $<


all: $(BIN) $(ENDLESS) $(SLOW)

$(ENDLESS): $(ENDLESS).c
	@echo "Building endless..."
	@$(CPP) $(ENDLESS).c -o $(ENDLESS)
	
$(SLOW): $(SLOW).c
	@echo "Building slow..."
	@$(CPP) $(SLOW).c -o $(SLOW)

$(BIN): $(OBJS)
	@echo "Linking $@..."
	@$(CPP) $(LNFLAGS) -o $@ $(OBJS)

$(OBJS): $(HEADERS)

clean:
	@echo "Cleaning files..."
	@-rm *.o *~ $(BIN) $(SLOW) $(ENDLESS)

#
# Bedeutung Makefile Makros:
#
# $< the name of the related file that caused the action.
# $* the prefix shared by target and dependent files.
# $@ is the name of the file to be made.
# $? is the names of the changed dependents.
