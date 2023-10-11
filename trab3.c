#include<unistd.h>

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int my_rank;
MPI_Status *status;

#define TAM_TAB 2048 //funcionando perfeitamente
#define GERACOES 2000

void definir_ranks(int *prox, int *ant, int qtde_procs){
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	*prox=(my_rank+1)%qtde_procs;
	*ant = (my_rank-1+qtde_procs)%qtde_procs;
}

void inicializa_grids(int **tabuleiro_a, int **tabuleiro_b, int tamanho){
	int i,j;
	for(i=0; i<tamanho; i++){
		tabuleiro_a[i]=(int*)malloc(sizeof(int)*TAM_TAB);
		tabuleiro_b[i]=(int*)malloc(sizeof(int)*TAM_TAB);
	}

	for(i=0;i<tamanho; i++ ){
		for(j=0;j<TAM_TAB; j++ ){
			tabuleiro_a[i][j]=0;
			tabuleiro_b[i][j]=0;
		}
	}

	if(my_rank==0){
		int lin = 1, col = 1;
		tabuleiro_a[lin][col+1] = 1;
		tabuleiro_a[lin+1][col+2] = 1;
		tabuleiro_a[lin+2][col  ] = 1;
		tabuleiro_a[lin+2][col+1] = 1;
		tabuleiro_a[lin+2][col+2] = 1;	

		lin =10, col = 30;
		tabuleiro_a[lin  ][col+1] = 1;
		tabuleiro_a[lin  ][col+2] = 1;
		tabuleiro_a[lin+1][col  ] = 1;
		tabuleiro_a[lin+1][col+1] = 1;
		tabuleiro_a[lin+2][col+1] = 1;
		
	}

}

int getVizinhos(int **tabuleiro_a, int *tabuleiro_prox, int *tabuleiro_ant, int i, int j, int tamanho){
	int contador = 0;
	int col_ant, col_prox, lin_ant, lin_prox;
	contador=0;
	col_ant = (j-1+TAM_TAB)%TAM_TAB;
	col_prox = (j+1)%TAM_TAB;

	lin_ant = (i-1);
	lin_prox = (i+1);

	if(lin_prox>tamanho-1){
		//usar tabuleiro_prox
		contador += tabuleiro_a[lin_ant][col_ant];
		contador += tabuleiro_a[lin_ant][j];
		contador += tabuleiro_a[lin_ant][col_prox];
		contador += tabuleiro_a[i][col_ant];
		contador += tabuleiro_a[i][col_prox];
		contador += tabuleiro_prox[col_ant];
		contador +=	tabuleiro_prox[j];
		contador += tabuleiro_prox[col_prox];

	}else if(lin_ant<0){
		//usar tabuleiro ant
		contador += tabuleiro_ant[col_ant];
		contador += tabuleiro_ant[j];
		contador += tabuleiro_ant[col_prox];
		contador += tabuleiro_a[i][col_ant];
		contador += tabuleiro_a[i][col_prox];
		contador += tabuleiro_a[lin_prox][col_ant];
		contador += tabuleiro_a[lin_prox][j];
		contador += tabuleiro_a[lin_prox][col_prox];

	}else{
		contador += tabuleiro_a[lin_ant][col_ant];
		contador += tabuleiro_a[lin_ant][j];
		contador += tabuleiro_a[lin_ant][col_prox];
		contador += tabuleiro_a[i][col_ant];
		contador += tabuleiro_a[i][col_prox];
		contador += tabuleiro_a[lin_prox][col_ant];
		contador += tabuleiro_a[lin_prox][j];
		contador += tabuleiro_a[lin_prox][col_prox];

	}
	//if(contador!=0){
		//printf("[%d](i,j):%d\n",my_rank,i,j,contador);
	//}
	return contador;
	
}

void atualizaGrid(int **grid, int **newgrid, int linha, int col, int tamanho, int vizinhos){
	if(grid[linha][col]==1 && (vizinhos == 2 || vizinhos == 3)){
		//Qualquer célula viva com dois ou três vizinhos deve sobreviver
		newgrid[linha][col] = 1;
	}else if(vizinhos == 3 && grid[linha][col]==0){
		//Qualquer célula morta com exatamente 3 vizinhos deve se tornar viva
		newgrid[linha][col] = 1;
	}else{
		newgrid[linha][col]=0;
	}
}

void printaGrid(int **tabuleiro_a, int tamanho, int iteracao){
	int i,j;
	for(i=0;i<tamanho; i++){
	printf("[%d](%d - %d) ",my_rank, i, iteracao);
		for(j=0;j<TAM_TAB; j++ ){
			printf("%d ", tabuleiro_a[i][j]);
		}
		printf("\n");
	}
}

void executa(int **tabuleiro_a, int **tabuleiro_b, int *tabuleiro_prox, int *tabuleiro_ant, int tamanho, int prox, int ant){
	int iteracao=0, i, j;

	for(iteracao=0; iteracao<GERACOES; iteracao++){
		//printf("[%d] iteração %d\n", my_rank, iteracao);
		int vizinhos=0;
		if(iteracao%2==0){
			MPI_Sendrecv (tabuleiro_a[tamanho-1], TAM_TAB, MPI_INT, prox, 0, tabuleiro_ant, TAM_TAB, MPI_INT, ant, 0, MPI_COMM_WORLD, status);
			MPI_Sendrecv (tabuleiro_a[0], TAM_TAB, MPI_INT, ant, 1, tabuleiro_prox, TAM_TAB, MPI_INT, prox, 1, MPI_COMM_WORLD, status);	
			for(i=0;i<tamanho; i++){
				for(j=0;j<TAM_TAB; j++ ){
					vizinhos = getVizinhos(tabuleiro_a, tabuleiro_prox, tabuleiro_ant, i, j, tamanho);
					//if(vizinhos !=0) printf("[%d](%d,%d)vizinhos: %d\n",my_rank,i,j,vizinhos);
					atualizaGrid(tabuleiro_a, tabuleiro_b,i,j,tamanho, vizinhos);
				}
			}
		}else{
			MPI_Sendrecv (tabuleiro_b[tamanho-1], TAM_TAB, MPI_INT, prox, 0, tabuleiro_ant, TAM_TAB, MPI_INT, ant, 0, MPI_COMM_WORLD, status);
			MPI_Sendrecv (tabuleiro_b[0], TAM_TAB, MPI_INT, ant, 1, tabuleiro_prox, TAM_TAB, MPI_INT, prox, 1, MPI_COMM_WORLD, status);	
			for(i=0;i<tamanho; i++){
				for(j=0;j<TAM_TAB; j++ ){
					vizinhos = getVizinhos(tabuleiro_b, tabuleiro_prox, tabuleiro_ant, i, j,tamanho);
					//if(vizinhos !=0) printf("[%d](%d,%d)vizinhos: %d\n",my_rank,i,j,vizinhos);
					atualizaGrid(tabuleiro_b, tabuleiro_a, i, j, tamanho, vizinhos);
				}
			}		
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

int getSobreviventes(int **grid, int tamanho){
	int i, j;
	int contador=0;
	for(i=0; i<tamanho; i++){
		for(j=0; j<TAM_TAB; j++){
			contador+= grid[i][j];
		}
	}
	return contador;
}

int main(int argc, char * argv[]) {
	
	if(argc == 1){
		printf("(Erro) Inicie da seguinte forma: ./main <quantidade de processos>\n");
		return 0;
	}
	int qtde_procs = argv[1][0]-48;
	int tamanho = TAM_TAB/qtde_procs;	

	MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	
	double total_time = 0.0;
	int prox, ant;
	total_time -= MPI_Wtime();
	definir_ranks(&prox, &ant, qtde_procs);
	MPI_Barrier(MPI_COMM_WORLD);
	
	int **tabuleiro_a = (int**) malloc(sizeof(int*)*tamanho);
	int **tabuleiro_b = (int**) malloc(sizeof(int*)*tamanho);
	int *tabuleiro_prox = (int*) malloc(sizeof(int*)*TAM_TAB);
	int *tabuleiro_ant = (int*) malloc(sizeof(int*)*TAM_TAB);
	int i,j;
	
	inicializa_grids(tabuleiro_a, tabuleiro_b, tamanho);
	MPI_Barrier(MPI_COMM_WORLD);
	executa(tabuleiro_a, tabuleiro_b, tabuleiro_prox, tabuleiro_ant,tamanho, prox, ant);
	
	/*
	//	Não execute isso!
	//	Estávamos usando apenas para visualizar em casos pequenos (ex TAM_TAB=16)
	for(i=0; i<qtde_procs; i++){
		MPI_Barrier(MPI_COMM_WORLD);
		if(my_rank==i)
			printaGrid(tabuleiro_a, tamanho, 9);
	}
	*/
	MPI_Barrier(MPI_COMM_WORLD);
	int qtde_final;
	if(GERACOES%2 == 0){
		qtde_final = getSobreviventes(tabuleiro_a, tamanho);
	}else{
		qtde_final = getSobreviventes(tabuleiro_b, tamanho);	
	}
	
	//printf("[%d]Sobreviventes na geração %d: %d\n", my_rank, GERACOES, qtde_final); 
	MPI_Barrier(MPI_COMM_WORLD);
	
	int total;
	MPI_Reduce(&qtde_final, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	total_time += MPI_Wtime();
	if(my_rank == 0)
		printf("Células vivas: %d\nTempo percorrido: %lf\n", total, total_time);
	/*
	int soma=0;
	for(i=0;i<tamanho; i++ ){
		for(j=0;j<TAM_TAB; j++ ){
			printf("\n%d", soma);
			soma += tabuleiro_a[i][j];
		}
	}

	printf("[%d]Soma: %d\n", my_rank, soma);
	*/
	MPI_Finalize();
}
