 /*

File: librub.c
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
#include "sysmacro.h"

/* @description -- Trasforma la la struttura dati studente (di tipo voce *) in un 
                   array di caratteri.
 @param r[] -- l'array di caratteri
 @param v -- puntatore ad una struttura di tipo voce.
 @return x -- ritorna 0 se tutto è andato bene o -1 in presenza di errore
 @modify -- modifica l'array passato per parametro che conterrà tutti i campi di v
             eccetto lo \0 finale che non viene memorizzato. */
int vocetorec(char p[], voce* v)
{

/*  copio 140 caratteri + \n finale. Ogni strncpy copia la lunghezza del campo compreso
lo \0 ma questo viene sovrascritto nella strncpy seguente   */


/*
Ho provato ad utilizzare questo controllo ad ogni operazione:
if(!p)
perror("aprendo prorub.dat");
return -1;

ma non stampa il file nel modo corretto..
*/


p=strncpy(p,(*v).cognome,LCOGN+1);

p=strncat(p,(*v).nome,LNOME+1);

p=strncat(p,(*v).citta,LCITT+1);

p=strncat(p,(*v).telefono,LTELE+1);

p=strncat(p,(*v).commento,LCOMM+1);


return 0;
}



/* @description -- serializza una voce della rubrica trasformando la
voce v in una stringa formattata (b) per la stampa su standard
output.
 @param b[] -- la stringa da formattare
 @param v -- puntatore ad una struttura di tipo voce che contiene i dati da formattare.
 @return x -- restituisce 0 se tutto è andato bene o -1 in presenza di errori 
 @modify -- modifica l'array passato per parametro che conterrà tutti i campi di v
	             formattati*/
             
int formatvoce(char c[], voce* v)
{


char * tmp;

/* copia il tipo di campo della voce */
strncpy(c,"Cognome: ",10);
/* formatta la voce eliminando gli spazi e lo /0 */
tmp = formatta_key(v->cognome);
/* aggiunge tutto nell'array compreso il ritorno carrello */
strncat(c,tmp,LCOGN);
strncat(c,"\n",1);

/* ripete la stessa operazione per i campi successivi */

strncat(c,"Nome: ",8 );
tmp = formatta_key(v->nome);
strncat(c,tmp,LNOME);
strncat(c,"\n",1);


strncat(c,"Citta': ",8);
tmp = formatta_key(v->citta);
strncat(c,tmp,LCITT);
strncat(c,"\n",1);


strncat(c,"Telefono: ",11);
tmp = formatta_key(v->telefono);
strncat(c,tmp,LTELE);
strncat(c,"\n",1);


strncat(c,"Commenti: ",11);
tmp = formatta_key(v->commento);
strncat(c,tmp,LCOMM);
strncat(c,"\n",1);


return 0;
}


/* @description -- cerca l'occorrenza del pattern b in tutti i
                   campi della voce v
 @param b[] -- il pattern 
 @param v -- puntatore ad una struttura di tipo voce. 
 @return  -- 1 se b occorre in uno dei campi di
             v (anche parzialmente) e 0 altrimenti. 
*/



int matchvoce(char pattern[], voce* v)
{

/* si utilizza una seconda funzione, match, a 3 argomenti che 
   verifica l'uguaglianza di tutte le sottostringhe del campo
   e il pattern b    */


/* I passi: 
   1 - copia un campo della rubrica in una stringa allocata dinamicamente a seconda della grandezza del campo
   2 - controlla, attraverso la funzione match, se il pattern è uguale a una delle possibili sottostringhe di 
       lunghezza uguale alla lunghezza di pattern del campo corrente.
   3- se c'è riscontro termina positivamente ritornando 1 altrimenti esamina il campo successivo.
       */


int riscontro;

/* la stringa che contiene il campo  */
char  campo_corrent1[41];
char  campo_corrent2[21];

char * campo_corrente = campo_corrent1;

strncpy(campo_corrente,(*v).nome,41);
riscontro=match(campo_corrente,pattern);
if(riscontro == 1)
return 1;

if(riscontro == -1)
return -1;

strncpy(campo_corrente,(*v).cognome,41);
riscontro=match(campo_corrente,pattern);
if(riscontro==1)
return 1;

if(riscontro == -1)
return -1;

 
campo_corrente = campo_corrent2;

strncpy(campo_corrente,(*v).citta,21);
riscontro=match(campo_corrente,pattern);
if(riscontro==1)
return 1;

if(riscontro == -1)
return -1;


strncpy(campo_corrente,(*v).telefono,21);
riscontro=match(campo_corrente,pattern);
if(riscontro==1)
return 1;

if(riscontro == -1)
return -1;


strncpy(campo_corrente,(*v).commento,21);
riscontro=match(campo_corrente,pattern);
if(riscontro==1)
return 1;

if(riscontro == -1)
return -1;


return 0;
}


/* @description -- confronta le varie sottostringhe di s con il pattern b
 @param campo_corrente -- il campo corrente.
 @param sub -- conterrà le varie sottostringhe di s della stessa lunghezza del pattern.
 @param pattern --  il pattern
 @return x -- 1 se c'è uguaglianza tra s e b, 0 altrimenti 
*/

 
int match(char *campo_corrente,char pattern[]){

/* guardia del while. Vale 0 se si raggiunge la fine del file o il matching tra s e p  */
int x=1;

/* la dimensione del pattern. Mi serve per calcolare le sottostringhe. */
int lung_pattern = strlen(pattern);

char * campo_corrente_format;

/* alloco la memoria per la sottostringa del campo */

char * sub;



    /* se la grandezza del campo corrente è minore di pattern non potranno mai esserci uguaglianza
        tra una sottostringa di campo_corrente e pattern */
//printf("campo_corrente : %s ,%d,  pattern: %s, %d \n", campo_corrente, strlen(campo_corrente),pattern, strlen(pattern));

   
   campo_corrente_format = formatta_key(campo_corrente);

/* controllo se  campo_corrente è minore di pattern . In questo caso termino */
   if(strlen(campo_corrente_format)<strlen(pattern)){
    return 0;   
   }

   if((sub = calloc(lung_pattern,sizeof(pattern))) == NULL)
      return -1;


    while(x) {
    /* ad ogni ciclo calcolo la sottostringa di s e la copio in b */
    sub = strncpy(sub, campo_corrente, lung_pattern);
    if(strcmp(sub,pattern) == 0){	
    return 1;
     free(sub);
	   } 
    /* mi sposto di un carattere in avanti se quest'ultimo non è il terminatore della stringa.  */
  if(*(campo_corrente+1)=='\0')    
    x=0;
    else
    campo_corrente += 1;
   }
     free(sub);
     return 0;


}



/* @description -- converte una stringa di caratteri (r), che
rappresenta una voce della rubrica telefonica in una struttura di tipo "voce".

 @param r -- la stringa r da convertire.
 @return v -- il puntatore alla struttura di tipo voce allocata
 @modify -- decrizione effetti su variabili globali, array etc*/


voce* rectovoce(char r[])
{

voce* v;

/* punta alla voce */
char * tmp=&r[0];

 if ( (v =malloc(sizeof(voce))) == NULL )
{
  perror("allocando v");
  return NULL;
}

/* punto al primo carattere di r e alloco 40 caratteri di buffer per tmp  */

strncpy((*v).cognome,tmp,40);
strncat((*v).cognome,"\0",1);
tmp += 40;



strncpy((*v).nome,tmp,40);
strncat((*v).nome,"\0",1);
tmp += 40;



strncpy((*v).citta,tmp,20);
strncat((*v).citta,"\0",1);
tmp += 20;

strncpy((*v).telefono,tmp,20);
strncat((*v).telefono,"\0",1);
tmp += 20;

strncpy((*v).commento,tmp,20);
strncat((*v).commento,"\0",1);

return v; 
}

/* funzione che elimina gli spazi e lo /0 finale dalla stringa passata per argomento  */
char * formatta_key(char * record){

int bool=1, i=0;
/* carattere corrente  */
char * corrente = malloc(1);


/* stringa destinazione */

char * r;

while(bool){
/* inizializzo il carattere corrente */
corrente = NULL;
corrente = malloc(1);
/* copio un carattere dentro corrente */
sprintf(corrente,"%d",record[i]);

/* se questo carattere è uno spazio controllo il carattere successivo(evito di eliminare cosi il secondo nome di un nome
composto, es. Diano Marina, Portobello di Gallura, New York) */

if(atoi(corrente) == 32){
corrente = NULL;
corrente = malloc(1);
sprintf(corrente,"%d",record[i+1]);
if(atoi(corrente) == 32){
/* assegno ad r i primi  caratteri escludendo l'i-esimo e l'i-esimo +1 ed assegno all'i-esimo carattere lo /0 */

r=(char *) malloc(i+1);
memset(r,'\0',i+1);
r=strncpy(r,record,i);
bool=0;
}
else
i++;
}
else
i++;
 
}

return r;
}
