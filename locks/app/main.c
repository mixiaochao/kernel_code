#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define CNT 100000000

/*
	互斥锁属于睡眠锁，拿不到锁的进程会睡眠。

	自旋锁睡眠非睡眠锁，拿不到锁的进程通过执行空语句，轮循的方式查看锁有没有释放。


	通常情况下加锁的粒度较小，保护的代码区域需要的时间很短的情况下使用自旋锁，否则使用互斥锁。


 */

/*
 	spinlock, mutex
 */

//pthread_mutex_t mutex;

pthread_spinlock_t spin;

void *do_thread_fool0 (void *data)
{
	int i;

	for (i = 0; i < CNT; i++) {
		//pthread_mutex_lock(&mutex);
		pthread_spin_lock(&spin);
		(*(int *)data)++;	
		pthread_spin_unlock(&spin);
		//pthread_mutex_unlock(&mutex);
	}

	return data;
}

void *do_thread_fool1 (void *data)
{
	int i;

	for (i = 0; i < CNT; i++) {
		//pthread_mutex_lock(&mutex);
		pthread_spin_lock(&spin);
		(*(int *)data)--;	
	//	pthread_mutex_unlock(&mutex);
		pthread_spin_unlock(&spin);
	}

	return data;
}

int main(void)
{
	int var = 9527;
	int ret;

	pthread_t tid[2];

//	pthread_mutex_init(&mutex, NULL);
	pthread_spin_init(&spin,  PTHREAD_PROCESS_SHARED);

	ret = pthread_create(&tid[0], NULL, do_thread_fool0, &var);
	if (ret != 0) {
		fprintf(stderr, "pthread_create failure...\n");
		exit(1);
	}

	ret = pthread_create(&tid[1], NULL, do_thread_fool1, &var);
	if (ret != 0) {
		fprintf(stderr, "pthread_create failure...\n");
		exit(1);
	}

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

//	pthread_mutex_destroy(&mutex);

	pthread_spin_destroy(&spin);

	printf("var = %d\n", var);

	return 0;
}

