android-app-memcpyspeed
=======================

#概要

NEONによるmemcpyの速度の評価用アプリ。

NEONによるmemcpyは[What is the fastest way to copy memory on a Cortex-A8?](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka13544.html)を参考にして実装した。


#ビルド
Android NDK r8e, Android-14, armeabi-v7a, NEON

	-march=armv7-a
	-mfloat-abi=softfp
	-mfpu=vfpv3-d16
	-O2
	-mfpu=neon 


#結果
標準のmemcpyとほとんど同じ。標準memcpyのほうが若干高速のような気がする。

	: -----------------------------------------------------
	: memcpy_neon 1048576 bytes 10000 times
	:  dst=0x7513f040 src=0x74746040
	: cputime avg. 301391nsec per memcpy
	: cputime avg. 294nsec per kB
	: -----------------------------------------------------
	: -----------------------------------------------------
	: memcpy 1048576 bytes 10000 times
	:  dst=0x7513f040 src=0x74746040
	: cputime avg. 284185nsec per memcpy
	: cputime avg. 277nsec per kB
	: -----------------------------------------------------

