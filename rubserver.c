/*

Server.c: implementazione di un Server per la gestione di una rubrica telefonica

versione: 1.0

autore: Cristiano Anselmi

fine: 14-7-2005


*/


#include "rubserver.h"
#include "librub.h"
#include "librubs.h"



/* la pipe del server */
char pipe_server[124];


/* nome del file passato come parametro */
char * file;

/* lista degli elementi, ciascuno  di tipo voce, per manovrare le voci della rubrica in memoria(cancellazione, inserzione, ricerca) */
elem * db;

/*array di caratteri che serve per memorizzare delle info riguardo le operazioni effettuate durante la sessione*/
char log_[1500];

int main(int argc, char * argv[])
{

/* descrittore della pipe */
int fd_pipe;


/* puntatore a struttura di tipo FILE a cui assegnerò il risultato della chiamata per l'apertura del file contenente la rubrica.   */
FILE * archivio;

/* ci informa della riuscita o meno di un operazione */
int stato;

/* assegno a file l'argomento della linea di comando che è l'indirizzo locale o assoluto della rubrica */
file= argv[1];

strncpy(log_,"Inizio", 6);
strncat(log_,"\n", 1);

/* controlla se il numero di argomenti è esattamente 2 */
if(argc != 2){
 perror("il numero di argomenti è diverso da due");
 exit(errno);
}


sprintf(pipe_server,"%s/prorub/tmp/clientserver",getenv("HOME"));


/* apre il file in lettura e...  */
  archivio=fopen(file ,"r");
  if ( archivio == NULL ) {
    perror("aprendo l'archivio");
     exit(errno);
  }
  
/*...controlla se il formato del database è idoneo */
stato = leggiArchivio(archivio,&db);
if(stato==-1){
WRITE("il formato di file non è valido");
return 0;
  }
else{
/* aggiungo informazioni alla variabile di log */
strncat(log_,"Archivio .prorub.dat letto", 26);
strncat(log_,"\n", 1);
}

fclose(archivio);
  
/* crea una pipe ./tmp/clientserver */
IFERROR(mkfifo(pipe_server,0666),"creazione pipe");
IFERROR(chmod(pipe_server,0666),"diritti pipe");

/* gestione segnale di terminazione */
IFERROR(sigaction(SIGTERM,NULL,&azione), "eseguendo la sigaction");
azione.sa_handler=gestore_INT;
IFERROR(sigaction(SIGTERM,&azione,NULL), "eseguendo la sigaction");
IFERROR(sigaction(SIGUSR2,&azione,NULL),"eseguo la sigaction");
azione.sa_handler=SIG_IGN;

WRITELN("Inizio");
do{

/* buffer del messaggio di 161 caratteri ricevuto dal client attraverso ./tmp/clientserver */
char buffer[161];
char * buf= buffer;

/* la pipe del client */
char pipe[20]=" ";
char * pipe_client= pipe;


/* il record */
char record[141];


IFERROR(fd_pipe = open(pipe_server,O_RDONLY), "aprendo pipe");
IFERROR(read(fd_pipe,buf,161),"leggendo da pipe nel server");

/* il server accetta dei messaggi dal client. All'arrivo di una richiesta lancia la funzione corrispondente e aggiorna
  il log. */

/* tre istruzioni per estrarre il nome della pipe del client dal buffer */
strncpy(pipe_client, buf ,21);

/* non considero il primo carattere che identifica il comando da eseguire  */
pipe_client++;

/* elimino gli spazi dalla pipe */
pipe_client= formatta_key(pipe_client);

/* sposto il puntatore "superando" la pipe */
buf+=21;
strncpy(record, buf, RECLEN);

/* faccio puntare buf all'inizio dell'array  */
buf = buffer;

 if(buf[0] == 'A'){
 stato = insert(pipe_client,record);
 strncat(log_, "Inserzione", 10);
 strncat(log_, "\n", 1);
}
 
 if(buf[0] == 'D'){
 /* considero la chiave senza paddings */
  char er[141]= " ";
  char * r= er;
 r = formatta_key(record);
 stato = delete(pipe_client , r);
 strncat(log_, "Cancellazione",13);
 strncat(log_, "\n", 1);
}

 if(buf[0] == 'Q'){
  char er[141]= " ";
  char * r= er;
 r = formatta_key(record);

 stato = cerca_rec(pipe_client, r);
 strncat(log_, "Interrogazione",14);
 strncat(log_, "\n", 1);

}

close(fd_pipe);

}
while(1==1);
return 0;
}


void gestore_INT(int sig){
FILE * archivio;
int status;

char fn[124];

/* stampa il log */

printf(log_);
printf("Termino...\n");printf("Registrazione dell'archivio\n");


/* all'arrivo del segnale di terminazione  il server ...*/

if(db != NULL)  {
  archivio=fopen(file,"w");
  if ( archivio == NULL ) {
    perror("aprendo l'archivio");
     exit(errno);
  }
  
/* registra le voci */
status = registraArchivio(archivio, db);



fclose(archivio);

}

/* elimina la pipe di ascolto ./tmp/clientserver  */
unlink(pipe_server);

/* elimino il file ../.tmp.dat utilizzato nella registra_archivio */
sprintf(fn,"%s/.tmp.dat",getenv("HOME"));
unlink(fn);

/* esce con codice -1 */
exit(-1);
}

/*
@description Inserisce un nuovo record nella rubrica
@param pipe_client, la stringa contenente il nome della pipe del client
@param rec , il record da aggiungere
@require 
@return 1 se va tutto bene, 0 altrimenti 
@modify conta, contatore elementi della rubrica
*/

int insert(char * pipe_client , char * rec){


/* il record. L'array contenente il record era già dichiarato e inizializzo nel main.
    In C gli array nei parametri vengono passati per riferimento e non per valore.
    Dunque, dichiaro un array di 141 caratteri e li copio dentro i valori a cui punta 
    il parametro rec, ossia i valori dell'array record nel main. */

char record[141];
/* status delle varie operazioni*/
int status; 
voce* v;


/* variabile di tipo studente che conterrà una nuova voce  e un puntatore ad una lista di elementi */


strncpy(record, rec ,141);

v = rectovoce(record);




status = push(&db,v);

/* rispondo al cliente in caso negativo se l'inserzione non và a buon fine */
if(status==-1){
rispondi(pipe_client, status,"NOI");
return 0;
  }

conta++;
rispondi(pipe_client, status,"OK");

  return 1;
  }



/*
@description Elimina un record nella rubrica
@param pipe_client, la stringa contenente il nome della pipe del client
@param r, la chiave.
@require 
@return 1 se va tutto bene, 0 altrimenti 
@modify conta, contatore elementi della rubrica
*/

int delete(char * pipe_client , char * r){


int status=0, bool=0;	

/* se db è vuoto allora non eseguo alcuna cancellazione. Da aggiungere */

if (conta != 0){
if(db->ptvoce->nome!=NULL){
status = cancella(r, &db);
 }
}

/* sposto il db di n posizioni avanti se sono stati cancellati i primi n record */
if (conta != 0){
while(bool == 0){
if(db->ptvoce->nome==NULL)
db=db->next;
else
bool = 1 ;
}

}


if(status==0){
rispondi(pipe_client, status,"NOC");
return 0;
  }

if(status==-1){
rispondi(pipe_client, status,"ERR");
return 0;
  }




rispondi(pipe_client, status,"OK");


   
   return 1;
}


/*
@description cerca un record nella rubrica
@param pipe_client, la stringa contenente il nome della pipe del client
@param r, la chiave.
@require 
@return 1 se va tutto bene, 0 altrimenti 
@modify 
*/

int cerca_rec(char * pipe_client , char * r){

/* puntatore all lista degli elementi trovati */
elem * er;

/* status comando e descrittore della pipe */
int status=0, fd,i;

/*stringa che contiene un campo voce della lista degli elementi trovati. Serve per l'invio al client  */
char  messaggio[141]= " ";
char * pi= messaggio;

/* messaggio di chiusura per il client */
char messaggio_fin[141]=" ";
/*int bool = 0;*/


/* RICERCO GLI ELEMENTI */

/* la funzione cerca memorizza dentro la lista er gli elementi di tipo voce trovati. */

if (conta != 0){
status = cerca(r, db, &er);
  }

if(status==0){
rispondi(pipe_client, status,"NOQ");
return 0;
  }

if(status==-1){
rispondi(pipe_client, status,"NOQ");
return 0;
  }


  /* INVIO GLI ELEMENTI */

     IFERROR(fd = open(pipe_client,O_WRONLY), "SERVER :aprendo pipe client");      


      /* attraverso questo ciclo while trasformo ogni campo voce dell'elemento di tipo elem in un  
         record, aggiungo lo /0
         e aggiungo in testa al messaggio A e spedisco tutto al client.
         "A" serve per differenziare il messaggio dal messaggio finale, identifica un record 
         trovato.
         "/0" identifica la chiusura della stringa. 
           */

      while(er){
  
      vocetorec(messaggio,(*er).ptvoce);
      messaggio[RECLEN]='\0';
      pi=concatena("A",messaggio);

      IFERROR(write(fd,pi,RECLEN+1), "scrivendo su pipeclient nel server");
      er= (*er).next;

      }
 

/* invio il messaggio di chiusura */

  strncpy(messaggio_fin, "STOP",4);
  for(i=4; i<141; i++)
  strncat(messaggio_fin," ",1);

  IFERROR(write(fd,messaggio_fin,141), "scrivendo su pipeclient nel server");
  IFERROR(close(fd), "chiudendo pipeserver nel client");


return 1;
}


/*
@description risponde al client con un messaggio di 141 caratteri
@param pipe, la pipe su cui scrivere
@param status, lo stato di un operazione dal quale dipenderà il tipo di messaggio
@param type, tipo di messaggio
@require 
@return
@modify
*/

void rispondi(char * pipe, int status, char * type){

int fd,i;
char messaggio[141]= " ";
char * p= messaggio;

if(status ==-1)
p=concatena("E",type);
 else{
if(status == 0) 
p=concatena("E",type);

else
strncpy(p,type, strlen(type));
}

for(i= strlen(p); i<141; i++)
strncat(p," ",1);


  IFERROR(fd = open(pipe,O_WRONLY), "in funzione rispondi:aprendo pipe client");
  IFERROR(write(fd,p,141), "scrivendo su pipeserver nel client");
  IFERROR(close(fd), "chiudendo pipeclient nel server");
  
  }
	 	
	
