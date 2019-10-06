/*
 * algorithm: odd-even transposition sort (alg. ~40 lines long)
 * author: jakub zak
 *
 */

 #include <mpi.h>
 #include <iostream>
 #include <fstream>
 
 using namespace std;

 #define TAG 0

 int main(int argc, char *argv[])
 {
    int numprocs;               //pocet procesoru
    int myid;                   //muj rank
    int neighnumber;            //hodnota souseda
    int mynumber;               //moje hodnota
    MPI_Status stat;            //struct- obsahuje kod- source, tag, error

    //MPI INIT
    MPI_Init(&argc,&argv);                          // inicializace MPI 
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);       // zjistíme, kolik procesů běží 
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);           // zjistíme id svého procesu 
 
    //NACTENI SOUBORU
    /* -proc s rankem 0 nacita vsechny hodnoty
     * -postupne rozesle jednotlive hodnoty vsem i sobe
    */
    if(myid == 0){
        char input[]= "numbers";                          //jmeno souboru    
        int number;                                     //hodnota pri nacitani souboru
        int invar= 0;                                   //invariant- urcuje cislo proc, kteremu se bude posilat
        fstream fin;                                    //cteni ze souboru
        fin.open(input, ios::in);                   

        while(fin.good()){
            number= fin.get();
            if(!fin.good()) break;                      //nacte i eof, takze vyskocim
            cout<<invar<<":"<<number<<endl;             //kdo dostane kere cislo
            MPI_Send(&number, 1, MPI_INT, invar, TAG, MPI_COMM_WORLD); //buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
            invar++;
        }//while
        fin.close();                                
    }//nacteni souboru

    //PRIJETI HODNOTY CISLA
    //vsechny procesory(vcetne mastera) prijmou hodnotu a zahlasi ji
    MPI_Recv(&mynumber, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat); //buffer,velikost,typ,rank odesilatele,tag, skupina, stat
    //cout<<"i am:"<<myid<<" my number is:"<<mynumber<<endl;
 
    //LIMIT PRO INDEXY
    int oddlimit= 2*(numprocs/2)-1;                 //limity pro sude
    int evenlimit= 2*((numprocs-1)/2);              //liche
    int halfcycles= numprocs/2;
    int cycles=0;                                   //pocet cyklu pro pocitani slozitosti
    //if(myid == 0) cout<<oddlimit<<":"<<evenlimit<<endl;


    //RAZENI------------chtelo by to umet pocitat cykly nebo neco na testy------
    //cyklus pro linearitu
    for(int j=1; j<=halfcycles; j++){
        cycles++;           //pocitame cykly, abysme mohli udelat krasnej graf:)

        //sude proc 
        if((!(myid%2) || myid==0) && (myid<oddlimit)){
            MPI_Send(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);          //poslu sousedovi svoje cislo
            MPI_Recv(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);   //a cekam na nizsi
            //cout<<"ss: "<<myid<<endl;
        }//if sude
        else if(myid<=oddlimit){//liche prijimaji zpravu a vraceji mensi hodnotu (to je ten swap)
            MPI_Recv(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat); //jsem sudy a prijimam

            if(neighnumber > mynumber){                                             //pokud je leveho sous cislo vetsi
                MPI_Send(&mynumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);       //poslu svoje 
                mynumber= neighnumber;                                              //a vemu si jeho
            }
            else MPI_Send(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);   //pokud je mensi nebo stejne vratim
            //cout<<"sl: "<<myid<<endl;
        }//else if (liche)
        else{//sem muze vlezt jen proc, co je na konci
        }//else

        //liche proc 
        if((myid%2) && (myid<evenlimit)){
            MPI_Send(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);           //poslu sousedovi svoje cislo
            MPI_Recv(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);    //a cekam na nizsi
            //cout<<"ll: "<<myid<<endl;
        }//if liche
        else if(myid<=evenlimit && myid!=0){//sude prijimaji zpravu a vraceji mensi hodnotu (to je ten swap)
            MPI_Recv(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat); //jsem sudy a prijimam

            if(neighnumber > mynumber){                                             //pokud je leveho sous cislo vetsi
                MPI_Send(&mynumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);       //poslu svoje 
                mynumber= neighnumber;                                              //a vemu si jeho
            }
            else MPI_Send(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);   //pokud je mensi nebo stejne vratim
            //cout<<"ls: "<<myid<<endl;
        }//else if (sude)
        else{//sem muze vlezt jen proc, co je na konci
        }//else
        
    }//for pro linearitu
    //RAZENI--------------------------------------------------------------------


    //FINALNI DISTRIBUCE VYSLEDKU K MASTEROVI-----------------------------------
    int* final= new int [numprocs];
    //final=(int*) malloc(numprocs*sizeof(int));
    for(int i=1; i<numprocs; i++){
       if(myid == i) MPI_Send(&mynumber, 1, MPI_INT, 0, TAG,  MPI_COMM_WORLD);
       if(myid == 0){
           MPI_Recv(&neighnumber, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat); //jsem 0 a prijimam
           final[i]=neighnumber;
       }//if sem master
    }//for

    if(myid == 0){
        //cout<<cycles<<endl;
        final[0]= mynumber;
        for(int i=0; i<numprocs; i++){
            cout<<"proc: "<<i<<" num: "<<final[i]<<endl;
        }//for
    }//if vypis
    //VYSLEDKY------------------------------------------------------------------

 
    MPI_Finalize(); 
    return 0;

 }//main

