FROM gcc:10.2.0

ENV SONAR_SCANNER_VERSION 4.2.0.1873
ENV PATH /opt/sonar-scanner-$SONAR_SCANNER_VERSION-linux/bin:/opt/build-wrapper-linux-x86:$PATH

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgtest-dev

RUN curl -SLo /opt/sonar-scanner.zip https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-$SONAR_SCANNER_VERSION-linux.zip 
RUN unzip -o /opt/sonar-scanner.zip -d /opt

RUN curl -SLo /opt/build-wrapper-linux-x86.zip https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip
RUN unzip -o /opt/build-wrapper-linux-x86.zip -d /opt/

