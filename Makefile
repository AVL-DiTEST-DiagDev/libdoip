CXX = g++

CPPFLAGS = -g -Wall -Wextra
LDFLAGS = -shared
TESTFLAGS = -g -L/usr/lib -lgtest -lgtest_main -lpthread

SRCPATH = src
INCPATH = include
BUILDPATH = build
TESTPATH = test

COMMONTARGET = libdoipcommon
SERVERTARGET = libdoipserver
CLIENTTARGET = libdoipclient

# List of all source files, separated by whitespace
COMMONSOURCE = $(wildcard $(COMMONTARGET)/$(SRCPATH)/*.cpp)
SERVERSOURCE = $(wildcard $(SERVERTARGET)/$(SRCPATH)/*.cpp)
CLIENTSOURCE = $(wildcard $(CLIENTTARGET)/$(SRCPATH)/*.cpp)

TESTSOURCE = TestRunner.cpp
TESTSOURCE += $(wildcard $(COMMONTARGET)/test/*.cpp)
TESTSOURCE += $(wildcard $(SERVERTARGET)/test/*.cpp)
TESTSOURCE += $(wildcard $(CLIENTTARGET)/test/*.cpp)

COMMONOBJS = $(patsubst $(COMMONTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH)/%.o, $(COMMONSOURCE))
SERVEROBJS = $(patsubst $(SERVERTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH)/%.o, $(SERVERSOURCE))
CLIENTOBJS = $(patsubst $(CLIENTTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH)/%.o, $(CLIENTSOURCE))

.PHONY: all clean

all: env $(BUILDPATH)/$(COMMONTARGET).so $(BUILDPATH)/$(SERVERTARGET).so $(BUILDPATH)/$(CLIENTTARGET).so test

env:
	mkdir -p $(BUILDPATH)

clean:
	rm -rf $(BUILDPATH)/*.*    

$(BUILDPATH)/%.o: $(COMMONTARGET)/$(SRCPATH)/%.cpp
	$(CXX) $(CPPFLAGS) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@
	
$(BUILDPATH)/%.o: $(SERVERTARGET)/$(SRCPATH)/%.cpp
	$(CXX) $(CPPFLAGS) -I $(SERVERTARGET)/$(INCPATH) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@
	
$(BUILDPATH)/%.o: $(CLIENTTARGET)/$(SRCPATH)/%.cpp
	$(CXX) $(CPPFLAGS) -I $(CLIENTTARGET)/$(INCPATH) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@
    
$(BUILDPATH)/$(COMMONTARGET).so: $(COMMONOBJS)
	$(CXX) $(CPPFLAGS) $^ $(LDFLAGS) -o $@
	
$(BUILDPATH)/$(SERVERTARGET).so: $(SERVEROBJS)
	$(CXX) $(CPPFLAGS) $^ $(LDFLAGS) -o $@
	
$(BUILDPATH)/$(CLIENTTARGET).so: $(CLIENTOBJS)
	$(CXX) $(CPPFLAGS) $^ $(LDFLAGS) -o $@
	
test:
	$(CXX) $(CPPFLAGS) -I $(COMMONTARGET)/$(INCPATH) -I $(SERVERTARGET)/$(INCPATH) -I $(CLIENTTARGET)/$(INCPATH) $(COMMONSOURCE) $(SERVERSOURCE) $(CLIENTSOURCE) -o runTest $(TESTSOURCE) $(TESTFLAGS) 
	