FROM debian

RUN apt-get update
RUN apt-get install -y --force-yes wget git make gnupg nano

# Unfortunately, this key isn't available over HTTPS.
RUN wget -O /tmp/llvm-snapshot.gpg.key http://apt.llvm.org/llvm-snapshot.gpg.key
RUN apt-key add /tmp/llvm-snapshot.gpg.key
RUN bash -c 'echo "deb http://apt.llvm.org/jessie/ llvm-toolchain-jessie-3.9 main" >> /etc/apt/sources.list'
RUN bash -c 'echo "deb-src http://apt.llvm.org/jessie/ llvm-toolchain-jessie-3.9 main" >> /etc/apt/sources.list'
RUN apt-get update
RUN apt-get install -y --force-yes clang-3.9 clang-3.9-doc libclang-common-3.9-dev libclang-3.9-dev libclang1-3.9 libclang1-3.9-dbg libllvm-3.9-ocaml-dev libllvm3.9 libllvm3.9-dbg lldb-3.9 llvm-3.9 llvm-3.9-dev llvm-3.9-doc llvm-3.9-examples llvm-3.9-runtime clang-format-3.9 python-clang-3.9 libfuzzer-3.9-dev


ENV CFLAGS="-O1 -g -fsanitize=address,bool,float-cast-overflow,integer-divide-by-zero,return,returns-nonnull-attribute,shift-exponent,signed-integer-overflow,unreachable,vla-bound -fno-sanitize-recover=all -fno-omit-frame-pointer -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION=1"
ENV CXXFLAGS="-O1 -g -fsanitize=address,bool,float-cast-overflow,integer-divide-by-zero,return,returns-nonnull-attribute,shift-exponent,signed-integer-overflow,unreachable,vla-bound -fno-sanitize-recover=all -fno-omit-frame-pointer -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION=1"
ENV LDFLAGS="-g -fsanitize=address,bool,float-cast-overflow,integer-divide-by-zero,return,returns-nonnull-attribute,shift-exponent,signed-integer-overflow,unreachable,vla-bound"
ENV CC="/usr/bin/clang-3.9"
ENV CXX="/usr/bin/clang++-3.9"
ENV ASAN_OPTIONS="exitcode=1,handle_segv=1,detect_leaks=1,leak_check_at_exit=1,allocator_may_return_null=1,detect_odr_violation=0"
ENV ASAN_SYMBOLIZER_PATH="/usr/lib/llvm-3.9/bin/llvm-symbolizer"

RUN bash -c 'echo "export CFLAGS=\"${CFLAGS}\"" >> /root/.bashrc'
RUN bash -c 'echo "export CXXFLAGS=\"${CXXFLAGS}\"" >> /root/.bashrc'
RUN bash -c 'echo "export LDFLAGS=\"${LDFLAGS}\"" >> /root/.bashrc'
RUN bash -c 'echo "export CC=\"${CC}\"" >> /root/.bashrc'
RUN bash -c 'echo "export CXX=\"${CXX}\"" >> /root/.bashrc'
RUN bash -c 'echo "export CXXFLAGS=\"${CXXFLAGS}\"" >> /root/.bashrc'
RUN bash -c 'echo "export ASAN_OPTIONS=\"${ASAN_OPTIONS}\"" >> /root/.bashrc'
RUN bash -c 'echo "export ASAN_SYMBOLIZER_PATH=\"${ASAN_SYMBOLIZER_PATH}\"" >> /root/.bashrc'

WORKDIR /test
RUN git clone git://thekelleys.org.uk/dnsmasq.git
WORKDIR /test/dnsmasq
# Patch Makefile so we can compile with ASAN
RUN sed -i'' 's/LDFLAGS       =/LDFLAGS       ?=/' Makefile
RUN sed -i'' 's/CFLAGS        =/CFLAGS        ?=/' Makefile
RUN make

# Only the last ENTRYPOINT or CMD is honored, so this can be overridden.
ENTRYPOINT /bin/bash
