#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int pthread_yield (void);
void *soma(void *th_num);


#define N_THREADS 3
#define TTL			3
#define LEN 128

typedef struct
{
	int pos_ini, tam, th_num;
	int *vet;
}thread_arg, *ptr_thread_arg;

pthread_mutex_t soma_value_lock = PTHREAD_MUTEX_INITIALIZER;
int soma_global; 

int main(int argc, char *argv[]) {
	// inicializa mutex
	int  num_values =0, num_threads=0;
	pthread_mutex_init(&soma_value_lock, NULL);
	num_values = atoi(argv[1]);
	num_threads = atoi(argv[2]);
	int values_per_thread = num_values / num_threads;
	thread_arg arguments[num_threads];
	arguments[0].vet = 	(int*) malloc(num_values*sizeof(int));
	for (int i = 0; i < num_values; ++i){
		arguments[0].vet[i] = (int)rand()%10;
		printf(" %i ", arguments[0].vet[i]);
	}
	for (int t = 1; t < num_threads; ++t)
	{
		printf("\n");
		arguments[t].vet = 	(int*) malloc(num_values*sizeof(int));
		for (int i = 0; i < num_values; ++i)
		{
			arguments[t].vet[i] = arguments[0].vet[i];
		}
	}

	

	pthread_t thread[num_threads];
	int t, result;
	char err_msg[LEN];
	int resto=num_values%num_threads;

	// cria threads
	for(t=0; t<num_threads; t++) {
		// printf("Criando thread %d\n", t);
		arguments[t].pos_ini = t*values_per_thread;
		if(t==num_threads-1){
			arguments[t].tam = values_per_thread+resto;
		}else{
			arguments[t].tam = values_per_thread;
		}
		arguments[t].th_num = t;
		if((result=pthread_create(&thread[t], NULL, soma, &(arguments[t])))) {
			strerror_r(result,err_msg,LEN);
			printf("Erro criando thread: %s\n",err_msg);
			exit(0);
		}
	}

	// espera threads concluírem
	for(t=0; t<num_threads; t++) {
		if((result=pthread_join(thread[t], NULL))) {
			strerror_r(result,err_msg,LEN);
			printf("Erro em pthread_join: %s\n",err_msg);
			exit(0);
		}
	}

	printf("Soma final: %d\n",soma_global);
	

//                TRECHO PARA VERIFICAR RESULTADO
	// int somaV=0;
	// for (int i = 0; i < num_values; ++i)
	// {
	// 	somaV+= arguments[0].vet[i];
	// }
	// printf("Valor correto: %i\n", somaV);
	// // libera mutex
	// pthread_mutex_destroy (&soma_value_lock);

	pthread_exit(NULL);

}

void *soma(void *arg){
	int i, soma_local=0;
	ptr_thread_arg arguments = (ptr_thread_arg)arg;
	int ini = arguments->pos_ini;
	int fim = arguments->tam + ini;
	int th_num = arguments->th_num;
	// int id=pthread_self();
	for (i = ini; i < fim; i++){
		printf("Th %d esperando\n", (int)th_num);
		pthread_mutex_lock(&soma_value_lock);
		printf("	Th %d entrou na regiao critica\n", (int)th_num);
		soma_global+= arguments->vet[i];
		pthread_mutex_unlock(&soma_value_lock);
		printf("	Th %d saiu da regiao critica\n", (int)th_num);
		pthread_yield();
	}
	printf("		Th %d Soma parcial: %d\n", (int)th_num, soma_local);

	pthread_exit(0);
}