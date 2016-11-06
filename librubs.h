typedef struct elem {
  voce* ptvoce;
  struct elem* next;
} elem;

int push(elem** db, voce* v);
int cancella(char key[], elem** db);
int cerca(char key[], elem* db, elem** r);
int stampaDatabase(FILE* uscita, elem* db);
int leggiArchivio(FILE* ingresso, elem** db);
int registraArchivio(FILE* uscita, elem* db);
