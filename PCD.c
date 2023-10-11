#include <stdio.h>
#include <stdlib.h>

void inicia(int mat[][2048]){
    for(int i=0;i<2048;i++){
        for(int j=0;j<2048;j++){
            mat[i][j]=0;
        }
    }
}

int main(){
    int status[2048][2048]; 
    int status2[2048][2048];
    inicia(status);
    return 0;
}

void checagem(int leitura[][], int escrita[][], int comeco, int fim){
    for(int i=comeco;i<fim;i++){
        for(int j=comeco;j<fim;j++){
            if(leitura[i][j]==1){//celula ta viva
                if(conta(leitura[i][j])==1){ //celular morre
                    escrita[i][j]=0;
                }
                else if(conta(leitura[i][j]==4)){//celular morre
                    escrita[i][j]=0;
                }
            }
            else if(leitura[i][j]==0){
                if(conta(leitura[i][j])==3){ //celular revive
                    escrita[i][j]=1;
                }
            }
        }
    }
}

void divide(int numthreads){
    int x=2048/numthreads;
    for(int i=0;i<numthreads;i++){
        //chama as threads de (pid-1)*x até (pid*x)-1
    } 
}