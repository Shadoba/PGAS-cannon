#include <upc.h>
#include <stdio.h>


shared  int A[THREADS];
shared  int B[THREADS]; 
shared  int C[THREADS];

#define MASTER 0

#define MATRIX_A "Matrix_A.txt"
#define MATRIX_B "Matrix_B.txt"
#define MATRIX_C "Output.txt"

#define LOG_ENABLE 0
#define TRACE_LOG(fmt, args...) do{if(LOG_ENABLE)fprintf(stdout, fmt, ##args);}while(0)

#define ERR_ENABLE 1
#define ERR_LOG(fmt, args...) do{if(ERR_ENABLE)fprintf(stderr, fmt, ##args);}while(0)

#define twoDimToOneDim(x, y, dim) (x*dim+y)

void loadInputFromFile(const int dim, const char * const MatA, const char * const MatB);
void saveOutputToFile(const int dim, const char * const MatC);

int main (int argc, char *argv[])
{
	int myid;
	int dim = (int)sqrt(THREADS);

  if( argc >= 3 )
  {
    loadInputFromFile(dim, argv[1], argv[2]);
  }
  else
  {
    loadInputFromFile(dim, MATRIX_A, MATRIX_B);
  }

	upc_barrier;
	
	upc_forall(myid=0; myid<THREADS; myid++; &C[myid])
  {
  	//row
  	int i = myid / dim;
    TRACE_LOG("i:%d\n", i);
  	//column
  	int j = myid % dim;
    TRACE_LOG("j:%d\n", j);
  
  	//initial skewing
  	int Ai = i;
  	int Aj = (j + i) % dim;
  	int Bi = (i + j) % dim;
  	int Bj = j;
  
    for(int shift=0; shift<dim; shift++) 
    {
    	//shift left
    	Aj = (Aj + 1) % dim;
    	//shift up
    	Bi = (Bi + 1) % dim;

  		int indexA =twoDimToOneDim(Ai, Aj, dim);
  		int indexB =twoDimToOneDim(Bi, Bj, dim);
    	C[myid] += A[indexA] * B[indexB];
      TRACE_LOG("C[%d]:%d; A[%d]:%d; B[%d]:%d\n", myid, C[myid], myid, A[myid], myid, B[myid]);
    }
	}

	upc_barrier;
	
  saveOutputToFile(dim, MATRIX_C);

}

void loadInputFromFile(const int dim, const char * const MatA, const char * const MatB) 
{
	  FILE *file;
	  int val = 0;
   	int count = 0;
  	char dummy;
  
   	if(MYTHREAD == MASTER)
    {
        TRACE_LOG("\n--- INPUT MATRICES ---\n\n");
    		file = fopen(MatA, "r");
        if(file == NULL)
        {
      	  ERR_LOG("%s\n", strerror( errno ));
          exit(EXIT_FAILURE);
        }
  
    		for(int i=0; i<dim; i++) 
        {
      			for(int j=0; j<dim; j++)
            {
          			if( EOF == fscanf(file,"%d",&val) )
                {
                  ERR_LOG("Za ma³a liczba elementów w macierzy\n"); 
    			        exit(0); 
          			}
                A[twoDimToOneDim(i,j,dim)] = val;
       			} 
    		}
    		fclose(file);

    		file=fopen(MatB, "r");
        if(file == NULL)
        {
      	  ERR_LOG("%s\n", strerror( errno ));
          exit(EXIT_FAILURE);
        }
        
    		for(int i=0; i<dim; i++)
        {
      			for(int j=0; j<dim; j++)
            {
          			if( EOF == fscanf(file,"%d",&val) )
                {
                  ERR_LOG("Za ma³a liczba elementów w macierzy\n"); 
    			        exit(0); 
          			}
                B[twoDimToOneDim(i,j,dim)] = val;
       			} 
    		}
    		fclose(file);
   }
}

void saveOutputToFile(const int dim, const char * const MatC)
{
	if(MYTHREAD == MASTER) 
  {
    FILE *file;
    file=fopen(MatC, "w");
    if(file == NULL)
    {
  	  ERR_LOG("%s\n", strerror( errno ));
      exit(EXIT_FAILURE);
    }

	  printf("Macierz wynikowa:\n");
    for(int i=0; i<THREADS; i++) 
    {
      if(i%dim == 0) 
      {
        printf("\n");
        fprintf(file,"\n");
      }
      printf("%d ",C[i]);
      fprintf(file,"%d ",C[i]);
    }
    fclose(file);
  }
}