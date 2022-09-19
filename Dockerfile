FROM ubuntu:22.04

RUN apt-get -qq update && apt -y -qq install gcc g++ cmake git >/dev/null

RUN git clone https://github.com/A-nnonymous/alnENV.git

RUN cd /alnENV/fftw-3.3.10 &&\
    echo "THIS MAY TAKE A LONG TIME, BE PATIENT PLEASE..."&&\
    ./configure  --enable-threads --enable-openmp --enable-avx2 --enable-float CFLAGS="-fPIC" >/dev/null&&\
    make -j >/dev/null && make install >/dev/null

RUN cd /alnENV/wavelib-master && mkdir build && cd build && cmake ../ >/dev/null &&\
    make -j >/dev/null && make install >/dev/null   

RUN mv /alnENV/wavelib-master/build/Bin/* /alnENV/fCWT/src/libwavelib&&\
    cd /alnENV/fCWT && mkdir build && cd build && cmake ../ >/dev/null &&\
    make -j >/dev/null && make install >/dev/null &&\
    cp /usr/local/lib/libfCWT*  /usr/lib


