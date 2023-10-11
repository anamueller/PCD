#include <stdio.h>
#include <pthread.h>

void cliente(void* i);
void servidor(void* i);

#define NUM_THREADS 4

long cont; // Variável global que se deseja incrementar
long local; // Variável local dos clientes

long respond;
long request;

int main(void){

	pthread_t t[NUM_THREADS];
	pthread_t t_server;
	int tid;
	long N;
	long i;		
	
	N = 10; // numero de interações
	
	local = N / NUM_THREADS; // numero de interações de cada thread
	cont = 0;
	request = 0;
	respond = 0;
	
	pthread_create(&t_server, NULL, servidor, NULL); // Cria thread do servidor
	for(i = 1; i <= NUM_THREADS; i++){ 
		pthread_create(&t[i-1], NULL, cliente, (void*) i);			
	}
	
	//pthread_create(&t_server, NULL, servidor, NULL);
	
	for(i = 0; i < NUM_THREADS; i++){
		pthread_join(t[i], NULL);
	}
	
	printf("cont = %ld\n", cont);
	return 0;
}

void cliente(void* i){
	
	long aux = (long) i;
	long j;	

	for(j = 0; j < local; j++) {
		while(respond != aux){request = aux;}
		while(respond != aux){request = aux;}
		cont++; // seção crítica
		respond = 0;
	}
	pthread_exit(NULL);
}

void servidor(void * i){
	while(1){
		while(request == 0){
            //wait
        }
		respond = request;		
		while(respond != 0){
            //wait
        }
		request = 0;
	}
	pthread_exit(NULL);
}