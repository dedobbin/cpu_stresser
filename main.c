#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <assert.h>

#define _STRESS_THREAD_USLEEP 1500

static int  _meassure_cpu_load = 1;
static long double _cpu_load;

static int _current_stress_level = 0;
static pthread_t * _stress_threads;
static int * _keep_stressing;

void* do_stress(void* param){
	int id = (int) param;
	_keep_stressing[id] = 1;
	while(_keep_stressing){
		int x = 4 + 5;
		pow(2,10);
		usleep(_STRESS_THREAD_USLEEP);
		//printf("hai from thread %d\n", id);
	}
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
    
            _cpu_load = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3])) * 100;
            printf("The current CPU utilization is : %Lf\n",_cpu_load);
     }
}

void set_cpu_stress(int new_stress_level)
{
	int i;
	if (new_stress_level < _current_stress_level){
		for(i = 0; i < _current_stress_level - new_stress_level; i++){
			int id = _current_stress_level - i;
			_keep_stressing[id] = 0;
			pthread_join(_stress_threads[id], NULL);
			
			_keep_stressing = realloc(_keep_stressing, new_stress_level*sizeof(*_keep_stressing));
			_stress_threads = realloc(_stress_threads, new_stress_level*sizeof(*_stress_threads));
		}
	}
	else if(new_stress_level > _current_stress_level){
		for (i = 0; i < new_stress_level; i++){
			_keep_stressing = realloc(_keep_stressing, new_stress_level*sizeof(*_keep_stressing));
			_stress_threads = realloc(_stress_threads, new_stress_level*sizeof(*_stress_threads));

			int id = _current_stress_level + i;
			pthread_create(&_stress_threads[_current_stress_level + i], NULL, do_stress, (void*)i);
		}
	}
	_current_stress_level = new_stress_level;
	printf("Stress level set to %d\n", _current_stress_level);
}

int main (int argc, char* argv[])
{
	assert(sizeof(int*)== sizeof (int));
	pthread_t meassure_thread;
	
	_stress_threads = malloc(sizeof(pthread_t)); 
	_keep_stressing = malloc(sizeof(int)); 
		
	pthread_create(&meassure_thread, NULL, meassure_cpu_load, NULL);
	
	int input;
	while(1){
		scanf("%d", &input);
		set_cpu_stress(input);
	}
	pthread_join(meassure_thread, NULL);
	return 0;
}




