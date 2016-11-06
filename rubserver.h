/*

Server.h: header di un Server per la gestione di una rubrica telefonica

versione: 1.0

autore: Cristiano Anselmi

data: 19-5-2005 fine: 05-06-2005


*/

/* headers  */
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

/* headers per gestione file  */
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>

/* gestione errori  */
#include "sysmacro.h"

/* utilità */
#include "util.c"

/* include per chiamate sui segnali */
#include <signal.h>

/* include per chiamate sui processi */
#include <sys/wait.h>



/* collezione di funzioni per la gestione della rubrica  */



/* macro */
#define TRUE 1
#define FALSE 0
#define MSG 161
#define ERROR -1
#define OK 1


/*variabile esterna (appartenente a librubs.c). Contatore dei record della rubrica */
extern int conta;
struct sigaction azione;

/* prototipi  */
int insert(char * , char *);
int delete(char *, char *);
int cerca_rec(char *, char *);
void rispondi(char *, int, char *);
void gestore_INT(int);
