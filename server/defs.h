#define MANOS_POR_NIVEL 5 //cantidad de manos por nivel de ciegas
#define P_INICIALES 1500 //puntos iniciales de cada jugador
#define MAX_LONG_NOMBRE 25//limite de longitud para el nombre
#define CANT_SIMULACIONES 20975//7//2 //400 //cantidad de simulaciones calculadora

struct sCarta{ //estructura de la carta, con palo y valor
	char carta;
	char palo[10];
};

typedef struct sCarta CARTA;

typedef struct sNode { //estructura nodo para la lista circular de cartas que sera el mazo
  CARTA D;
  struct sNode * next;
} NODE;

int calcular_jugada(CARTA *,CARTA *,CARTA *,int, int);

void calcular_jugada1 (CARTA *,CARTA *,char *);

void calcular_jugada2(CARTA *,char *,int,int);

void calcular_jugada3(CARTA *,char *);

int calcular_jugada4(char *,char *);

char carta_a_valor(char);

int escalera(char *);

int color(CARTA *);

void results(char *);

char valor_a_carta(char);

void reparto (CARTA *,CARTA *,CARTA *);

void fill_mazo(NODE **);

NODE * shuffle_mazo (NODE **,int);

void print_mazo(NODE *);

void deal_cards(NODE **,CARTA *,CARTA *,CARTA *);

void insert_node(NODE **, NODE *);

int count_nodes(NODE *);

NODE * select_node(NODE *,int);

void detach_node(NODE **,NODE *);

void print_cards (CARTA *,int);

int showdown(CARTA *,CARTA *,CARTA *,int *,int *,int,int,int,int);

void no_showdown(int *,int *,int,int);

int random_num (int);

int max_multiplo (int);

void delete_list(NODE **);

int recibir_nombre(int, char *);

int recibir_conf(int);

int enviar_conf(int);

int enviar_cartas(int,int,CARTA *,CARTA *,CARTA *);

int preflop(int *, int *, int *,int *,int *,int,int,int,int,int);

int postflop(int *, int *, int *,int *,int *,int,int,int,int,int);

int enviar_datos(int,char *,int *,int,int);

int turno(char *,int *,int);

int enviar_result(int,int,char *,char*);

int enviar_ganador(int,int,int);

void calculadora (void);

void calculadora_1(void);

NODE * select_node_by_user(NODE *,char,char);

int realizar_sim (CARTA *,CARTA *,CARTA *);

void ICM (void);

void ICM_1 (void);

double calculo_de_Eq$(int *,int,int,int);

double suma (int *,int,int,int);
