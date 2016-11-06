#define CAPACITA 200
#define LNOME 40
#define LCOGN 40
#define LCITT 20
#define LTELE 20
#define LCOMM 20
#define RECLEN (LNOME+LCOGN+LCITT+LTELE+LCOMM)

typedef struct {
 char nome[LNOME+1];
 char cognome[LCOGN+1];
 char citta[LCITT+1];
 char telefono[LTELE+1];
 char commento[LCOMM+1];
} voce;

voce* rectovoce(char r[]);
int vocetorec(char r[], voce* v);
int formatvoce(char b[], voce* v);
int matchvoce(char b[],voce* v);
char * formatta_key(char * );
int match(char *,char[]);
