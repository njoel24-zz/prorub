/*

File: librubs.c
Autore: Cristiano Anselmi
Programma: ProRub
Versione: 1.0 in data 07-05-2005

Dichiaro che il programma è opera originale dell'autore:

Cristiano Anselmi.

*/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "librub.h"
#include "librubs.h"




#define ERROR -1
#define OK 1



/* PROBLEM:
nella funzione stampaDatabase utilizzo una guardia attiva sullo stato di db nel ciclo.
Se uso lo stesso procedimento nelle funzioni cerca e registra database 
va in Segmentation fault e sono costretto a utilizzare una variabile globale.
*/

/*  il numero dei record scritti. Var globale. Utilizzata dalla funzione cerca per
conoscere il numero di record da processare nella ricerca, dalla funzione
LeggiArchivio che la aggiorna e dalla funzione registra archivio che la usa per
sapere quante voci registrare sul file  */
int conta =0;

    
/* @description inserisce un elemento in cima alla pila
   @param db -- il puntatore al puntatore alla testa della rubrica 
   @param v --  v e' il puntatore alla voce da memorizzare.
   @modify --   
   @require --
   @return --  ritorna -1 se si e' verificato un errore e 0 se
               tutto si e' svolto correttamente.
*/
int push(elem** db, voce* v)
{

    elem * t;

    if (( t = (elem *) malloc(sizeof(elem))) == NULL )
	 return ERROR;
    else{
   
       t->ptvoce = v;
       
   	 t->next = *db;
   	  *db=t;

	   return OK;
    }
}



/* @description cancella un elemento dalla lista.
   @param key -- la chiave
   @param db --  e db e' il puntatore al puntatore alla testa della rubrica
   @modify --   
   @require --
   @return --  il numero degli elementi cancellati, oppure
               restituisce  -1 se si e' verificato un errore.
*/
int cancella(char key[], elem** db)
{

int riscontro_match=0;
/*contatore elementi trovati */
int cont=0;

/* contatore elementi processati */
int conta_processati=0;
/* puntatore alla testa della rubrica */

elem * t= *db;
	
/*  puntatore all'elemento precedente a quello esaminato .Mi serve per ridirigere il puntatore */

elem * prec;


    if (( prec  = (elem *) malloc(sizeof(elem))) == NULL )
	 return ERROR;
    else {


do{

  if((conta != 0)&&(conta != conta_processati)){
  riscontro_match=matchvoce(key, (*t).ptvoce);
}
   else
    return cont;
  
if(riscontro_match==-1)
return -1;



  if(riscontro_match == 1){


     (*prec).next = (*t).next;

     free(t);

     conta--;

     cont++;


     t=(*prec).next;
     
   }
 /* nuovo  else*/    
else{
   prec = t;

   t=(*t).next;	
     conta_processati ++;
 }

/* riazzero riscontro_match nel caso conta è 0 */
riscontro_match = 0;

}
while(1==1);


}

return cont;
}


/* @description cerca un elemento nella lista.
   @param key -- la chiave
   @param db --  il puntatore  alla testa della rubrica
   @modify --   
   @require --
   @return --  il numero degli elementi trovati, oppure
               restituisce  -1 se si e' verificato un errore.
*/

int cerca(char key[], elem* db, elem** r)
{

/*  lista di elementi trovati di tipo elem. Insieme a r implementa una pila come
nella funzione push   */
elem * lista_trovati;

/* l'esito della funzione matchvoce()  */

int riscontro=0;

/* il numero degli elementi trovati  */
int trovati=0;

/*contatore degli elementi della rubrica */
int i=0;

/* dichiaro NULL il puntatore deferenziato da r 
per evitare che l'assegnamento   lista_trovati->next = *r; dia risultati insoliti  */
*r= NULL;



do{
 
  riscontro=matchvoce(key, (*db).ptvoce);
  i++;
  
  if(riscontro){
  
  /* alloco uno spazio di memoria equivalente ad un elemento di tipo elem e lo assegno a listatrovati */

    if (  (   lista_trovati =(elem *)  malloc(sizeof(elem))) == NULL )
	 return ERROR;
    else {


  
  /*assegno a lista trovati l'elemento */
  lista_trovati->ptvoce = db->ptvoce;
  
  /* sposto il resto della lista avanti di una posizione. Ragionamento a pila*/
  lista_trovati->next = *r;
   
   *r=lista_trovati;
 
   trovati++;

   }
}
      db=(*db).next;	

   }
  while(i!=conta);



  

  
 return trovati;
}

/* @description stampa la rubrica db, sul file uscita utilizzando la formatvoce() per la
                formattazione.
   @param uscita -- il file su cui scrivere.
   @param db --  e' il puntatore alla testa della rubrica
   @modify --   
   @require --
   @return --  il numero dei record stampati oppure -1 se si e'
               verificato un errore.*/

int stampaDatabase(FILE* uscita, elem* db)
{
     char pb[RECLEN+100];


     while(db){
     formatvoce(pb,(*db).ptvoce);
     fprintf(uscita,	"%s\n",pb);
      db= (*db).next;
      }
return 0;
}




/* @description legge il file ingresso che contiene il database e inserisce tutti i record nella rubrica
                puntata da db.
   @param ingresso -- il file 
   @param db --  e db e' il puntatore al puntatore alla testa della rubrica
   @modify --   la variabile globale conta
   @require --
   @return -- -1 se si e' verificato un errore e il numero dei
               record letti se tutto e' andato a buon fine.*/

int leggiArchivio(FILE* ingresso, elem** db)
{

/* la riga del file */
char r[141]; 


voce* v;

/*stringa di riscontro alla lettura del file*/
char * s;

/*variabile globale */
conta=0;

while((s = fgets(r,143,ingresso)) != NULL){ 
   /* trasforma la voce della rubrica memorizzata in una stringa in una struttura di tipo voce  */
   v = rectovoce(r);
   /*inserimento a pila */
   push(db, v);
   conta++;
 }
 
 return conta;
}




/* @description scrive tutti i record della rubrica db nel file uscita
   @param uscita -- il file da scrivere 
   @param db --  e db e' il puntatore alla testa della rubrica
   @modify --   
   @require --
   @return --  restituisce -1 se si e'
               verificato un errore e il numero dei record scritti altrimenti.*/


int registraArchivio(FILE* uscita, elem* db)
{

/* Descrizione:

   1 - Apre un file temporaneo(%./tmp.dat), scrive le voci della rubrica in questo file.
   2 - Riapre il file temporaneo e ricarica le voci in una lista di elementi di tipo elem (db2)
   3 - le riscrive nuovamente nel file destinazione
       
Nota: se scrivo direttamente le voci nel file , queste verranno scritte nell'ordine inverso rispetto all'originale, causa
l'ulitizzo della struttura dati a pila.

*/

/*Contatore: conta le voci della rubrica processate fin quando non risulta essere uguale a conta, var globale che tiene 
conto del numero di elementi della rubrica*/
int i=0;

/*array di caratteri che conterrà una riga del file, ossia una voce della rubrica */
char r[RECLEN+2];	


elem * db2= NULL;

/*struttura che riferisce al file restituita dalla funzione fopen */
FILE * archivio;

/* array di caratteri che conterrà il nome del file temporaneo(compreso di percorso) che serve per scrivere i dati della rubrica */
char fn[124];
char er[141]; 



voce* v;

char * s;

/*scrivo su fn il path del file temporaneo  */
sprintf(fn,"%s/.tmp.dat",getenv("HOME"));


/* apro il file */
archivio=fopen(fn,"w");
  if ( archivio == NULL ) {
    perror(fn);
    return 1;
  }


/* scrivo sul file le voci della rubrica */

 while(i!=conta){
  vocetorec(r,db->ptvoce);	

  r[RECLEN]='\0';

  fputs(r,archivio);
  fputs("\n",archivio);

  db = db->next; 
  i++;
}

   fclose(archivio);


archivio=fopen(fn,"rw");
  if ( archivio == NULL ) {
    perror(fn);
    return 1;
  }
  
conta=0;


/*ricarica il file in una struttura di tipo elem  */
while((s = fgets(er,143,archivio)) != NULL){ 


   v = rectovoce(er);

   push(&db2, v);
    
   conta++;
 }


fclose(archivio);


/* riazzero il contatore*/
i=0;

/*riscrivo nuovamente, questa volta sul file vero e proprio */
while(i!=conta){
  vocetorec(r,db2->ptvoce);	
  r[RECLEN]='\0';
  fputs(r,uscita);
  fputs("\n",uscita);
  db2 = db2->next; 
  i++;
}




return i;
}
 

