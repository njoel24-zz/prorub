/*

Client.h: header di un client per operare su una rubrica telefonica

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

#include "util.h"

/* utilità */
#include "util.c"



/* macro che definiscono la lunghezza del campo corrente della struttura studente */
#define LUNG1 40
#define LUNG2 20

void insert();
void delete(char[]);
void search(char[]);
void scrivi_pipe_server(char[], char[]);
int controllo_no_numeri(char *);
int controllo_no_lettere(char *);
char * insert_campo(char *);
