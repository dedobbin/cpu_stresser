#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <assert.h>

#define _STRESS_THREAD_USLEEP 1500

static int  _meassure_cpu_load = 1;
static int _current_stress_level = 0;
static int  _keep_stressing = 0;
static long double _current_cpu_load;

pthread_t* stress_threads;
int num_stress_threads = 0;

void* do_stress(void* param){
	int id = (int)param;
	while(_keep_stressing){
		int x = 4 + 5;
		pow(2,10);
		usleep(_STRESS_THREAD_USLEEP);
	}
	//printf("threads %d ends\n", id);
}


void* meassure_cpu_load(void* param)
{
    long double a[4], b[4];
    FILE *fp;
    char dump[50];

    while(_meassure_cpu_load){
            fp = fopen("/proc/stat","r");
            fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
            fclose(fp);
            sleep(4);
    
            fp = fopen("/proc/stat","r");
            fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
            fclose(fp);
    
            _current_cpu_load = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3])) * 100;
            printf("The current CPU utilization is : %Lf\n",_current_cpu_load);
     }
}

void halt_all_stress_threads()
{
	_keep_stressing = 0;
	int i;
	for(i=0; i < num_stress_threads; i++){
		pthread_join(stress_threads[i], NULL);
	}
	num_stress_threads = 0;
	_keep_stressing = 1;
}

void set_cpu_stress(int new_stress_level)
{
	halt_all_stress_threads();

	//For now, stress level directly is num threads
	stress_threads = realloc(stress_threads, new_stress_level * sizeof(pthread_t));
	int i;
	for(i=0; i<new_stress_level;i++){
		//Dirty passing int as address
		pthread_create(&stress_threads[i], NULL, do_stress, (void*)i);
		num_stress_threads++;
	}
	_current_stress_level = new_stress_level;
	printf("stress level to %d\n", new_stress_level);
}

int main (int argc, char* argv[])
{
	pthread_t meassure_thread;		
	pthread_create(&meassure_thread, NULL, meassure_cpu_load, NULL);
	
	//We want to realloc in set_cpu_stress, proper to do a malloc before that although in this case doesn't do anything
	stress_threads = (pthread_t*)malloc(sizeof(pthread_t));

	int input;
	do{
		scanf("%d", &input);
		if (input > 0){
			set_cpu_stress(input);
		}
	}while(input > 0);
	_meassure_cpu_load = 0;
	pthread_join(meassure_thread, NULL);
	halt_all_stress_threads();
	free(stress_threads);
	printf("bye");
	return 0;
}




