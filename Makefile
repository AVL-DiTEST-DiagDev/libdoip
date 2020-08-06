CXX = g++
GCOV = gcov

CPPFLAGS = -g -Wall -Wextra -std=c++11
CPPFLAGS_COV = $(CPPFLAGS) -O0 --coverage
LDFLAGS = -shared
LDFLAGS_TEST = -L/usr/lib -lgtest -lgtest_main -lpthread
LDFLAGS_COV = $(LDFLAGS_TEST) --coverage

SRCPATH = src
TESTPATH = test
INCPATH = include
BUILDPATH = build
BUILDPATH_TEST = build_test
BUILDPATH_COV = build_cov

TESTBINARY = runTest
COVBINARY = $(TESTBINARY)

COMMONTARGET = libdoipcommon
SERVERTARGET = libdoipserver
CLIENTTARGET = libdoipclient

# Library source and object files
COMMONSOURCE = $(wildcard $(COMMONTARGET)/$(SRCPATH)/*.cpp)
SERVERSOURCE = $(wildcard $(SERVERTARGET)/$(SRCPATH)/*.cpp)
CLIENTSOURCE = $(wildcard $(CLIENTTARGET)/$(SRCPATH)/*.cpp)
ALLSOURCE = $(COMMONSOURCE) $(SERVERSOURCE) $(CLIENTSOURCE)

COMMONOBJS = $(patsubst $(COMMONTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH)/%.o, $(COMMONSOURCE))
SERVEROBJS = $(patsubst $(SERVERTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH)/%.o, $(SERVERSOURCE))
CLIENTOBJS = $(patsubst $(CLIENTTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH)/%.o, $(CLIENTSOURCE))
ALLOBJS = $(COMMONOBJS) $(SERVEROBJS) $(CLIENTOBJS)

# Test source and object files
TESTSOURCE = TestRunner.cpp
TESTOBJS = $(patsubst %.cpp, $(BUILDPATH_TEST)/%.o, $(TESTSOURCE))

COMMONSOURCE_TEST = $(wildcard $(COMMONTARGET)/$(TESTPATH)/*.cpp)
SERVERSOURCE_TEST = $(wildcard $(SERVERTARGET)/$(TESTPATH)/*.cpp)
CLIENTSOURCE_TEST = $(wildcard $(CLIENTTARGET)/$(TESTPATH)/*.cpp)

COMMONOBJS_TEST = $(patsubst $(COMMONTARGET)/$(TESTPATH)/%.cpp, $(BUILDPATH_TEST)/%.o, $(COMMONSOURCE_TEST))
SERVEROBJS_TEST = $(patsubst $(SERVERTARGET)/$(TESTPATH)/%.cpp, $(BUILDPATH_TEST)/%.o, $(SERVERSOURCE_TEST))
CLIENTOBJS_TEST = $(patsubst $(CLIENTTARGET)/$(TESTPATH)/%.cpp, $(BUILDPATH_TEST)/%.o, $(CLIENTSOURCE_TEST))
ALLOBJS_TEST = $(COMMONOBJS_TEST) $(SERVEROBJS_TEST) $(CLIENTOBJS_TEST)

# Coverage source and object files
COVSOURCE = $(TESTSOURCE)
COVOBJS = $(patsubst %.cpp, $(BUILDPATH_COV)/%.o, $(COVSOURCE))

COMMONOBJS_COV = $(patsubst $(COMMONTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH_COV)/%.o, $(COMMONSOURCE))
SERVEROBJS_COV = $(patsubst $(SERVERTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH_COV)/%.o, $(SERVERSOURCE))
CLIENTOBJS_COV = $(patsubst $(CLIENTTARGET)/$(SRCPATH)/%.cpp, $(BUILDPATH_COV)/%.o, $(CLIENTSOURCE))
ALLOBJS_COV = $(COMMONOBJS_COV) $(SERVEROBJS_COV) $(CLIENTOBJS_COV)

.PHONY: all clean test coverage

all: env $(BUILDPATH)/$(COMMONTARGET).so $(BUILDPATH)/$(SERVERTARGET).so $(BUILDPATH)/$(CLIENTTARGET).so test

env:
	mkdir -p $(BUILDPATH)
	mkdir -p $(BUILDPATH_TEST)
	mkdir -p $(BUILDPATH_COV)

clean:
	rm -rf $(BUILDPATH)/*.*
	rm -rf $(BUILDPATH_TEST)/*
	rm -rf $(BUILDPATH_COV)/*.*
	rm -rf $(TESTBINARY)
	rm -rf *.gcov *.gcda *.gcno
	rm -rf testOutput.xml

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
	
# Test

test: $(BUILDPATH_TEST)/$(TESTBINARY)

$(BUILDPATH_TEST)/%.o: $(COMMONTARGET)/$(TESTPATH)/%.cpp
	$(CXX) $(CPPFLAGS) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@
	
$(BUILDPATH_TEST)/%.o: $(SERVERTARGET)/$(TESTPATH)/%.cpp
	$(CXX) $(CPPFLAGS) -I $(SERVERTARGET)/$(INCPATH) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@
	
$(BUILDPATH_TEST)/%.o: $(CLIENTTARGET)/$(TESTPATH)/%.cpp
	$(CXX) $(CPPFLAGS) -I $(CLIENTTARGET)/$(INCPATH) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@
	
$(BUILDPATH_TEST)/%.o: $(TESTSOURCE)
	$(CXX) $(CPPFLAGS) $(TESTFLAGS) -I $(COMMONTARGET)/$(INCPATH) -I $(SERVERTARGET)/$(INCPATH) -I $(CLIENTTARGET)/$(INCPATH) -c $< -o $@

$(BUILDPATH_TEST)/$(TESTBINARY): $(TESTOBJS) $(ALLOBJS) $(ALLOBJS_TEST)
	$(CXX) $^ $(LDFLAGS_TEST) -o $@


# Coverage

coverage:
	$(GCOV) -a $(ALLSOURCE) -o $(BUILDPATH_COV)
	mv *.gcov $(BUILDPATH_COV)

coverage_build: $(BUILDPATH_COV)/$(TESTBINARY)

$(BUILDPATH_COV)/%.o: $(COMMONTARGET)/$(SRCPATH)/%.cpp
	$(CXX) $(CPPFLAGS_COV) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@
	
$(BUILDPATH_COV)/%.o: $(SERVERTARGET)/$(SRCPATH)/%.cpp
	$(CXX) $(CPPFLAGS_COV) -I $(SERVERTARGET)/$(INCPATH) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@
	
$(BUILDPATH_COV)/%.o: $(CLIENTTARGET)/$(SRCPATH)/%.cpp
	$(CXX) $(CPPFLAGS_COV) -I $(CLIENTTARGET)/$(INCPATH) -I $(COMMONTARGET)/$(INCPATH) -fPIC -c $< -o $@

$(BUILDPATH_COV)/%.o: $(TESTSOURCE)
	$(CXX) $(CPPFLAGS) $(TESTFLAGS) -I $(COMMONTARGET)/$(INCPATH) -I $(SERVERTARGET)/$(INCPATH) -I $(CLIENTTARGET)/$(INCPATH) -c $< -o $@

$(BUILDPATH_COV)/$(TESTBINARY): $(COVOBJS) $(ALLOBJS_COV) $(ALLOBJS_TEST)
	$(CXX) $^ $(LDFLAGS_COV) -o $@

install:
	install -d /usr/lib/libdoip
	install -d /usr/lib/libdoip/include
	install build/*.so /usr/lib/libdoip
	install libdoipcommon/include/*.h /usr/lib/libdoip/include
	install libdoipserver/include/*.h /usr/lib/libdoip/include
	
