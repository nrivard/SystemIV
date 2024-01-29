# syntax=docker/dockerfile:1

FROM ubuntu:latest
RUN \
	apt -y update && \
	apt -y install wget make man git gcc g++ libgmp-dev libmpfr-dev libmpc-dev xz-utils texinfo srecord
RUN \
	mkdir /tmp/downloads && \
	mkdir /tmp/build && \
	mkdir /tmp/build/binutils && \
	mkdir /tmp/build/gcc
RUN \
	cd /tmp/downloads && \
	wget --quiet http://sun.hasenbraten.de/vasm/release/vasm.tar.gz && \
	wget --quiet https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.xz && \
	wget --quiet https://ftp.gnu.org/pub/gnu/gcc/gcc-13.1.0/gcc-13.1.0.tar.xz && \
	tar -xf vasm.tar.gz && \
	tar -xf binutils-2.40.tar.xz && \
	tar -xf gcc-13.1.0.tar.xz
RUN \
	cd /tmp/downloads/vasm && \
	make CPU=m68k SYNTAX=mot && \
	mv vasmm68k_mot /usr/local/bin/
RUN \
	cd /tmp/build/binutils && \
	../../downloads/binutils-2.40/configure --target=m68k-elf --enable-gold=yes && \
	make && \
	make install
RUN \
	cd /tmp/build/gcc && \
	../../downloads/gcc-13.1.0/configure --target=m68k-elf --enable-languages=c --disable-libssp --disable-debug --disable-dependency-tracking --disable-silent-rules --disable-nls --disable-libstdcxx --without-headers && \
	make && \
	make install
RUN rm -rf /tmp/downloads
RUN rm -rf /tmp/build
RUN mkdir /Code
