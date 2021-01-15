#!/bin/bash
if [ $# -lt 1 ]
then
	echo "Usage: $0 <Sonar Login Token> [<git ref or branch name>]"
	exit 1
fi

# If a second parameter was given, extract the Branch or Pull Request Identifier to pass it on to Sonar
SONAR_BRANCH=""
if [ $# -gt 1 ]
then
	# In case of a git ref pointing to a branch, pass on the branch name
	if [ ${2:0:11} == 'refs/heads/' ]
	then
		SONAR_BRANCH="-Dsonar.branch.name=${2:11}"
	# In case of a git ref pointing to a pull request, pass on the pull request Key
	elif [ ${2:0:10} == 'refs/pull/' ]
	then
		PULLREQUEST_KEY=$2
		PULLREQUEST_KEY=${PULLREQUEST_KEY:10}
		PULLREQUEST_KEY=${PULLREQUEST_KEY%/*}
		SONAR_BRANCH="-Dsonar.pullrequest.key=$PULLREQUEST_KEY"
	# In case of any other git ref, pass on the ref without the prefix 'refs/'
	elif [ ${2:0:5} == 'refs/' ]
	then
		SONAR_BRANCH="-Dsonar.branch.name=${2:5}"
	# In case something else then a git ref was given, just pass on the parameter as it is
	else
		SONAR_BRANCH="-Dsonar.branch.name=$2"
	fi
fi

docker build -t libdoipbuild .
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild build-wrapper-linux-x86-64 --out-dir bw-output make clean coverage_build
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild ./build_cov/runTest --gtest_output="xml:./testOutput.xml"
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild make coverage
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild sonar-scanner -Dsonar.login="$1" $SONAR_BRANCH

