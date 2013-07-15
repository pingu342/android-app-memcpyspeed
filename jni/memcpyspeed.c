#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <jni.h>
#include <android/log.h>

//#define USE_MEMCMP (1)

void *align(void *p, unsigned int align)
{
	long tmp = (long)p % align;
	p = (void *)((long)p - tmp + align);
	return p;
}

struct timespec diff_timespec(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}

void *memcpy_neon(void *dst, const void *src, size_t len)
{
	asm volatile (
			"NEONCopyPLD:\n\t"
			"PLD [%0, #0xC0]\n\t"
			"VLDM %0!,{d0-d15}\n\t"
			"VSTM %1!,{d0-d15}\n\t"
			"SUBS %2,%2,#0x80\n\t"
			"BGE NEONCopyPLD\n\t"
			: "=&r" (src), "=&r" (dst), "=&r" (len)
			: "0" (src), "1" (dst), "2" (len)
			: "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "d10", "d11", "d12", "d13", "d14", "d15"
		);
}

unsigned long long timespec_to_nsec(struct timespec ts)
{
	unsigned long long nsec;
	nsec = ts.tv_sec * 1000000000;
	nsec += ts.tv_nsec;
	return nsec;
}

void test_usleep(int us)
{
	int i;
	struct timespec start, end, d;

	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "-----------------------------------------------------");
	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "%s %d usec", __FUNCTION__, us);

	clock_gettime(CLOCK_MONOTONIC, &start);
	usleep(us);
	clock_gettime(CLOCK_MONOTONIC, &end);
	d = diff_timespec(start, end);

	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "cputime %llunsec (%dsec + %dnsec)", timespec_to_nsec(d), d.tv_sec, d.tv_nsec);
	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "-----------------------------------------------------");
}

void test_memcpy(void *src, int size, int times, void *(*_memcpy)(void*, const void*, size_t), const char *name)
{
	int i;
	struct timespec start, end, d;
	void *dst = malloc(size+0x40);
	void *dst_align = align(dst, 0x40);
	unsigned long long avg=0;

	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "-----------------------------------------------------");
	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "%s %d bytes %d times", name, size, times);
	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", " dst=%p src=%p", dst_align, src);

	for (i=0; i<times; i++) {
#ifdef USE_MEMCMP
		memset(dst_align, 0, size);
#endif /*USE_MEMCMP*/
		clock_gettime(CLOCK_MONOTONIC, &start);
		_memcpy(dst_align, src, size);
		clock_gettime(CLOCK_MONOTONIC, &end);
#ifdef USE_MEMCMP
		if (memcmp(dst_align, src, size) != 0) {
			__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "memcmp error.");
		}
#endif /*USE_MEMCMP*/
		d = diff_timespec(start, end);
		avg += timespec_to_nsec(d);
	}
	avg /= times;
	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "cputime avg. %llunsec per memcpy", i, avg);
	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "cputime avg. %llunsec per kB", i, avg/(size/1024));

	free(dst);

	__android_log_print(ANDROID_LOG_DEBUG, "sakalog", "-----------------------------------------------------");
}

void Java_jp_saka_memcpyspeed_MemcpySpeedActivity_memcpyspeedtest(JNIEnv *pEnv, jobject thiz)
{
	int size=1024*1024;
	void *p=malloc(size+0x40);
	void *p_align=align(p, 0x40);
	memset(p_align, 0x55, size);
	test_usleep(1000000);
	test_memcpy(p_align, 1024*1024, 10000, memcpy_neon, "memcpy_neon");
	test_memcpy(p_align, 1024*1024, 10000, memcpy, "memcpy");
	free(p);
}

