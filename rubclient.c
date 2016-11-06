/*

Client.c: implementazione di un client per operare su una rubrica telefonica

versione: 1.0

autore: Cristiano Anselmi

fine: 14-7-2005

*/


#include "rubclient.h"
#include "librub.h"




/* stringa da inviare al Server*/
char  richiesta[161];
char  pipe_client[20]= "tmp/"; 

int main(int argc, char * argv[])
{

/* stringa che conterrà il pid del client  */
char  pid_char[4];

/* creo la stringa che identifica la pipe */
sprintf(pid_char,"%d",getpid());
strncat(pipe_client,pid_char,strlen(pid_char));

/* crea una pipe ./tmp/ */
IFERROR(mkfifo(pipe_client,0666),"creazione pipe");
IFERROR(chmod(pipe_client,0666), "diritti pipe");



/* interpreto il comando e lo invio al server. Richiamo la funzione corrispondente. */

if(strncmp(argv[1],"-d",2) == 0){
if(argc != 3){
 perror("il numero di argomenti è diverso da tre");
 exit(errno);
}
 delete(argv[2]);
}

if(strncmp(argv[1],"-q",2) == 0){
if(argc != 3){
 perror("il numero di argomenti è diverso da tre");
 exit(errno);
}

 search(argv[2]);
}

if(strncmp(argv[1],"-i",2) == 0){
if(argc != 2){
 perror("il numero di argomenti è diverso da due");
 exit(errno);
}

 insert();
}

return 0; 
}

/*
@description richiede al server l'inserimento di un record.
@param 
@require 
@return x 
@modify
*/

void insert(){

/* dichiaro un buffer che conterrà il record di 141 */
char   buf[RECLEN +1]; 
char * p= buf;


/*  puntatore, rilasciato dalla funzione insert_campo */
char * t;

/* variabile di riscontro per la presenza di caratteri invalidi */
int i=0;

/* serve per dividere i campi in seguito alla lettura dallo STDIN  */
int fd;
int fd_pipe;


WRITELN("Inserzione");

/*  setto il buffer p con RECLEN spazi e aggiungo \0 e \n alla fine */
p = memset(buf,' ', RECLEN);
p[RECLEN]='\0';



     /* CAMPO NOME  */

t = insert_campo("Nome");
  p+=40;
  strncpy(p, t, strlen(t)-1);
  p = buf;


     /* CAMPO COGNOME'  */

t = insert_campo("Cognome");
  strncpy(p, t, strlen(t)-1);
  p+=80;



     /* CAMPO CITTA'  */
   
t = insert_campo("Citta'");
  strncpy(p, t, strlen(t)-1);
  p+=20;


    /* CAMPO TELEFONO */

t = insert_campo("Telefono");
  strncpy(p, t, strlen(t)-1);
  p+=20;

   /* CAMPO COMMENTO  */

t = insert_campo("Commento");
  strncpy(p, t, strlen(t)-1);
  p+=20;


/* preparo il messaggio da spedire al server. A + pipe_client + eventuali paddings + messaggio  */

    strncpy(richiesta, "A" ,1); 
    strncat(richiesta, pipe_client,20);

    for(i=strlen(richiesta);i<21;i++)
    strncat(richiesta," ",1);


    strncat(richiesta,buf, strlen(buf));

/* scrivo tutto al server e aspetto una risposta */


  IFERROR(fd_pipe = open("./tmp/clientserver",O_WRONLY),"aprendo pipeserver nel client");
  IFERROR(write(fd_pipe,richiesta,161), "scrivendo su pipeserver nel client");
  IFERROR(close(fd_pipe), "chiudendo pipeserver nel client");

  /* attendo la risposta  */
  IFERROR(fd = open(pipe_client,O_RDONLY),"aprendo pipeclient nel client");

  /* read bloccante */
    IFERROR(read(fd,richiesta,141), "leggendo da pipeclient nel client");
  

  if(strncmp(richiesta,"OK",2)==0)
     puts("Terminazione corretta");

  IFERROR(close(fd), "chiudendo pipeclient nel client");
  unlink(pipe_client);	
      
}


/*
@description richiede al server l'inserimento di un record.
@param campo, il campo corrente da inserire
@require 
@return il valore del campo
@modify
*/
char * insert_campo(char * campo){

int status;

/* è il buffer che contiene il valore del campo */
char tmp[40];
char * t = tmp;

/* array di caratteri per la formattazione della stampa */
char formato_stampa[13];
char * f = formato_stampa;
f = memset(f,' ',12); 

/* formatto il buffer temporaneo */
  t = memset(t, '\0' ,40);

   
   sprintf(formato_stampa,"%s", campo);
   formato_stampa[strlen(campo)]= ' ';
   formato_stampa[8] = ' ';
   formato_stampa[9] = '-';
   formato_stampa[10]= '>';
   formato_stampa[11]= ' ';
   formato_stampa[12]= '\0';

   printf("%s", formato_stampa);
 
/* con la fgets : campo+/n+/0  */
        t = fgets(t, 40, stdin); 

/* verifico con una stampa il valore del campo */
        printf("%s", t);

/*    controllo la presenza di caratteri invalidi a seconda del campo */

     if (strncmp(campo, "Telefono", strlen(campo))==0)
    status = controllo_no_lettere(t);
    else
    status = controllo_no_numeri(t);
    if (status != -1 ){ 
    unlink(pipe_client);
      exit(-1);
     }     

return t;
} 

/*
@description rischiesta eliminazione di un record dalla rubrica
@param key
@param pipe_client
@require 
@return 
@modify 
*/

void delete(char  key[]){

/* conterrà la conferma di eliminazione(OK) o la negazione */
int fd;



/* controllo se la chiave è minore di 40 caratteri */
if(strlen(key)<=40){


/* scrive sulla pipe */
scrivi_pipe_server(key, "D");

WRITE("Rimozione ");
WRITELN(key);

   	    
   /* attendo la risposta */
 



  
 IFERROR(fd = open(pipe_client,O_RDONLY),"aprendo pipeserver nesdl client");


/* questo ciclo controlla se.... */
while(read(fd,richiesta,141) > 0){
  	
/* le prime quattro lettere sono uguali a ENOC. Se si termina chiudendo il descrittore della pipe ed eliminandola */
          if(strncmp(richiesta,"ENOC",4)==0){
       WRITELN("Terminazione corretta");
       IFERROR(close(fd), "chiudendo pipeserver nel client");
      unlink(pipe_client);	
       exit(1);

       }  
      else{
  /* le prime quattro lettere sono uguali a EERR. Se si termina chiudendo il descrittore della    pipe ed eliminandola */
      if(strncmp(richiesta,"EERR",4)==0){
      WRITELN("Terminazione anomala");
       IFERROR(close(fd), "chiudendo pipeserver nel client");
      unlink(pipe_client);	
       exit(1);

      }
      else{
/* altrimenti termina regolarmente chiudendo il descrittore della pipe ed eliminandola */
            WRITELN("Terminazione corretta");
       IFERROR(close(fd), "chiudendo pipeserver nel client");
      unlink(pipe_client);	
       exit(1);

       }
      
       }
     
 }

 }
}
/*
@description richiesta di ricerca di un elemento della rubrica
@param key
@param pipe_client
@require
@return 
@modify 
*/

void search(char key[]){

/* descrittore della pipe del client */
int  fd;

/* conta il numero di voci riscontrate */
int conta =1;

/* struttura che conterrà la voce della rubrica corrente riscontrata durante la ricerca del server  */
voce * v;

 
if(strlen(key)<=40){



  scrivi_pipe_server(key, "Q");
    
    /* attendo la risposta dal server  */


   IFERROR(fd = open(pipe_client,O_RDONLY),"CLIENT: aprendo pipe_client");
   WRITE("Interrogazione ");
   WRITELN(key);

   /* ciclo di lettura */

  do{

    /* ad ogni ciclo inizializzo:
                                 la variabile intera LETTI che conta i bytes letti dalla read sulla pipe del client. 
				 la stringa RICHIESTA e il corrispondente puntatore P che conterrà la  stringa inviata dal server.

    */

    int letti=0;
    char richiesta[RECLEN+100];
    char * p= richiesta;
    

      IFERROR(letti=read(fd,p,141), "CLIENT: leggendo da pipe_client");
      
      /* se ci sono errori termina chiudendo descrittore della pipe ed elimandola */         
      if((strncmp(p,"ENOQ",4)==0)|(strncmp(p,"EERR",4)==0)){
      WRITELN("Terminazione corretta");
      IFERROR(close(fd), "chiudendo pipeserver nel client");
      unlink(pipe_client);	
      exit(1);
       }  
 
      /* altrimenti.. */
       else{
       /* dichiaro la stringa messaggio che conterrà la voce */
       char messaggio[141];
       if(richiesta[0] == 'A'){
       p++;
      /* copio, trasformo , formatto e stampo */
      strncpy(messaggio, p, 141);
      v=rectovoce(messaggio);
      formatvoce(p,v);
      puts(p); 
      conta++;
    }

      /*se i primi 4 caratteri della stringa inviata dal server sono uguali a STOP termina */
      if(strncmp(p, "STOP",4)==0){
       puts("Terminazione corretta");
      IFERROR(close(fd), "chiudendo pipeserver nel client");
      unlink(pipe_client);	
       exit(1);
     }  
    }
  }while(1==1);
   

	IFERROR(close(fd), "chiudendo pipeserver nel client");
        
    }

    }
    

/*
@description funzione di utilità per le funzioni delete e search per la presenza del . Scrive la stringa command + pipe_client + paddings + key + paddings  sulla pipe del server
@param key, la chiave
@param command, il comando da eseguire sul Server
@param pipe_client, il nome  della pipe del client che il Server deve conoscere.
@require
@return void
@modify 
*/

void scrivi_pipe_server(char  key[], char command[]){

     char rich[161]= " ";
     int fd_pipe, i;


       /* copio nell'array il comando */
        strncpy(rich, command ,1); 
/* aggiungo la pipe con paddings */
        strncat(rich, pipe_client, 20);
        

          for(i=strlen(rich);i<21;i++)
        strncat(rich," ",1);

/* aggiungo la chiave con paddings */        
        strncat(rich, key, strlen(key)); 
        
        for(i=strlen(rich);i<161;i++)
        strncat(rich," ",1);

  IFERROR(fd_pipe = open("./tmp/clientserver",O_WRONLY),"aprendo pipeserver nel client");
  IFERROR(write(fd_pipe,rich,161), "scrivendo su pipeserver nel client");
  IFERROR(close(fd_pipe), "chiudendo pipeserver nel client");
    }


/*
@description funzione di ultilità. Verifica se nella stringa passata come parametro sono presenti numeri
@param p , la stringa
@require
@return 
@modify 
*/

int controllo_no_numeri(char * p){
int i=0;

for(i=0; i<strlen(p) ; i++){
if((p[i]=='0')|(p[i]=='1')|(p[i]=='2')|(p[i]=='3')|(p[i]=='4')|(p[i]=='5')|(p[i]=='6')|(p[i]=='7')|(p[i]=='8')|(p[i]=='9'))
return (i+1);
 }
 return -1;
  }

/*
@description funzione di ultilità. Verifica se nella stringa passata come parametro sono presenti lettere
@param p, la stringa
@require
@return 
@modify 
*/

int controllo_no_lettere(char * p){
int i=0;

for(i=0; i<strlen(p) ; i++){
if((p[i]=='a')|(p[i]=='b')|(p[i]=='c')|(p[i]=='d')|(p[i]=='e')|(p[i]=='f')|(p[i]=='g')|(p[i]=='h')|(p[i]=='i')|(p[i]=='l')|(p[i]=='m')|(p[i]=='n')|(p[i]=='o')|(p[i]=='p')|(p[i]=='q')|(p[i]=='r')|(p[i]=='s')|(p[i]=='t')|(p[i]=='u')|(p[i]=='v')|(p[i]=='z'))
return (i+1);
 }
 return -1;
  }
