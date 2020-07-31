# libdoip
C/C++ library for Diagnostics over IP (DoIP)

### CI Status
* Master: ![CI status Master](https://github.com/AVL-DiTEST-DiagDev/libdoip/workflows/.github/workflows/ci.yml/badge.svg?branch=master)
* Dev: ![CI status Dev](https://github.com/AVL-DiTEST-DiagDev/libdoip/workflows/.github/workflows/ci.yml/badge.svg?branch=dev)

### Installing library for Diagnostics over IP

1. To install the library on the system, first get the source files with:
```
git clone https://github.com/GerritRiesch94/libdoip
```

2. Enter the directory 'libdoip' and build the library with:
```
make
```

3. To install the builded library into `/usr/lib/libdoip` use:
```
sudo make install
```

### Installing Google Test Framework

In order to compile the unit tests, the Google Test Framework is required to be installed on the system.

1. Install the default `libgtest-dev` package with:
```
sudo apt-get install libgtest-dev
```
2. Enter the directory in the bash of the installed package which is by default `/usr/src/gtest` and execute:
```
sudo cmake CMakeLists.txt
```
If this command finish successfully, build the static libraries with:
```
sudo make
```
3. If the previous step was successfully, you should have `libgtest.a` and `libgtest_main.a` in the gtest directory.
Copy the libraries into the `/usr/lib` directory with:
```
sudo cp libgtest.a /usr/lib
```
and
```
sudo cp libgtest_main.a /usr/lib
```

4. The Google Test Framework is now ready to use to compile tests.

##### Test if Google Test Framework works

This section will change as soon the Makefile is ready to use with the project.

1. Assuming you have a .cpp file with the main method and a simple test you can compile the test file with:
```
g++ -o testRun testFile.cpp -g -L/usr/lib/ -lgtest -lgtest_main -lpthread
```
If this finish successfully you should have a executable called `testRun` in the directory.

2. To execute the test and receive a xml file type:
```
./testRun --gtest_output="xml:./testOutput.xml"
```
