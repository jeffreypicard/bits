/*
 * false_sharing.c
 *
 * gcc -Wall -pthread -O2 -D DCACHE1_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE` false_sharing.c
 *
 * Author: Jeffrey Picard (jeff@jeffreypicard.com)
 */

#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define V_SIZE 8
#define N 1000000000

#if !defined(DCACHE1_LINESIZE) || !DCACHE1_LINESIZE
	#ifdef DCACHE1_LINESIZE
		#undef DCACHE1_LINESIZE
	#endif
	#define DCACHE1_LINESIZE 64
#endif
#define ____cacheline_aligned	__attribute__((aligned(DCACHE1_LINESIZE)))

void *compute(void *args)
{
	//const unsigned int V_MASK = V_SIZE - 1;
	int *v = (int *) args;
	int i = 0;
	for (i = 0; i < N; i++) {
		//v[i & V_MASK] = i;
		v[i % V_SIZE] = i;
	}
	return 0;
}

int
main(void)
{
	//int v1[V_SIZE];
	//int v2[V_SIZE];
	int v1[V_SIZE] ____cacheline_aligned;
	int v2[V_SIZE] ____cacheline_aligned;
	pthread_t t1, t2;

	printf("%p, %p\n", v1, v2);

	memset(v1, 0, V_SIZE);
	memset(v2, 0, V_SIZE);

	pthread_create(&t1, NULL, compute, v1);
	pthread_create(&t2, NULL, compute, v2);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	
	return 0;
}
