CXX = g++

CPPFLAGS = -g -Wall -Wextra
LDFLAGS = -shared

SRCPATH = src
INCPATH = include
BUILDPATH = build

# List of all source files, separated by whitespace
SOURCEFILES = example.cpp

OBJS = $(subst .cpp,.o,$(SOURCEFILES))

.PHONY: all clean

all: $(BUILDPATH)/libdoip.so

clean:
	rm -rf $(BUILDPATH)/*.*    

$(BUILDPATH)/$(OBJS): $(SRCPATH)/$(SOURCEFILES)
	$(CXX) $(CPPFLAGS) -I $(INCPATH) -fPIC -c $< -o $@
    
$(BUILDPATH)/libdoip.so: $(BUILDPATH)/$(OBJS)
	$(CXX) $(CPPFLAGS) $^ $(LDFLAGS) -o $@