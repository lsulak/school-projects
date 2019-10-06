/*
  "Hello World" s využitím MPI
 */
 #include <mpi.h>
 #include <stdio.h>
 #include <string.h>
 
 #define BUFSIZE 128
 #define TAG 0
 
 int main(int argc, char *argv[])
 {
   char idstr[32];
   char buff[BUFSIZE];
   int numprocs;
   int myid;
   int i;
   MPI_Status stat; 
 
   MPI_Init(&argc,&argv); /* inicializace MPI */
   MPI_Comm_size(MPI_COMM_WORLD,&numprocs); /* zjistíme, kolik procesů běží */
   MPI_Comm_rank(MPI_COMM_WORLD,&myid); /* zjistíme id svého procesu */
 
   /* Protože všechny programy mají stejný kód (Same Program, Multiple Data – SPMD) rozdělíme činnost
      programů podle jejich ranku. Program s rankem 0 rozešle postupně všem zprávu a přijme od všech
       odpověď 
   */
   if(myid == 0)
   {
     printf("%d: We have %d processors\n", myid, numprocs);
     for(i=1;i<numprocs;i++)
     {
       sprintf(buff, "Hello %d! ", i);
       MPI_Send(buff, BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
     }
     for(i=1;i<numprocs;i++)
     {
       MPI_Recv(buff, BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD, &stat);
       printf("ODPOVED %d: %s\n", myid, buff);
     }
   }
   else
   {
     /* obdržíme zprávu od procesu s rankem 0: */
     MPI_Recv(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
     sprintf(idstr, "Processor %d ", myid);
     strcat(buff, idstr);
     strcat(buff, "OK\n");
     /* Odpovíme na zprávu: */
     MPI_Send(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
   }
 
   MPI_Finalize(); 
   return 0;
 }

