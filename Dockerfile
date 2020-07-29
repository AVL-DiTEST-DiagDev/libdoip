FROM gcc:10.2.0

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgtest-dev

