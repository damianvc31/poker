#define MANOS_POR_NIVEL 5 //header igual que el correspondiente al server (salvo los prototipos de funciones)
#define P_INICIALES 1500
#define MAX_LONG_NOMBRE 25

struct sCarta{
	char carta;
	char palo[10];
};

typedef struct sCarta CARTA;

void results(char *);

char valor_a_carta(char);

void print_cards (CARTA *,int);

void print_puntos(int,int,int,int);

int showdown(CARTA *,CARTA *,CARTA *,int *,int *,int,int,int,char *,int);

void no_showdown(int *,int *,int,int,int);

int enviar_nombre(char *, int , char *);

int enviar_conf(char *, int);

int recibir_conf(char *, int);

int recibir_cartas(char *,int,CARTA *, CARTA *, CARTA *);

int preflop(int *,int *,int *,int *,int *,int,int,int,int,char *,int);

int postflop(int *,int *,int *,int *,int *,int,int,int,int,char *,int);

int recibir_datos(int *,char *,int *,char *,int);

int turno(char *,int *,int,int,int,char *,int,char);

int recibir_result(char *,int,char *,char *); 

int recibir_ganador(char *,int);
