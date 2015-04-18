#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>
//////////////////////////////////////////////
//                DEFINES                   //
//////////////////////////////////////////////

//Estados
enum estados{
	ESTADO_CRIACAO,
	ESTADO_APTO,
	ESTADO_EXECUCAO,
	ESTADO_BLOQUEADO,
	ESTADO_DESTRUICAO
};

//Min's
#define MIN_CICLOS_HD   200
#define MIN_CICLOS_VIDEO 100
#define MIN_CICLOS_PROCESSO 100
#define MIN_CICLOS_IMPRESSORA 500

//Max's
#define MAX_CICLOS_EXEC       50
#define MAX_CICLOS_HD         300
#define MAX_CICLOS_VIDEO      200
#define MAX_CICLOS_PROCESSO   300
#define MAX_CICLOS_IMPRESSORA 600
#define MAX_PROCESSOS         999

#define SOLICITAR_HD          0
#define SOLICITAR_VIDEO       1
#define SOLICITAR_IMPRESSORA  2

//////////////////////////////////////////////
//                STRUCTS                   //
//////////////////////////////////////////////
struct processo{
	struct processo * anterior;
	struct processo * proximo;
	int pid;
	int totalCiclos;
	int ciclos_executados;
	int estado;
};

struct item_fila{
	struct processo * atual;
	struct item_fila * proximo;
	int cls_rec_exec;
	int cls_rec_total;
};

//////////////////////////////////////////////
//                PROTÓTIPOS                //
//////////////////////////////////////////////
int sortearEntre( int min, int max);
int left_spc( int len_str, int space );
void criarProcesso(int pid);
void mostrarLog();
void mostrarLog_aptos();
void adicionar_fila_apto( struct processo * processo );
void adicionar_fila_hd( struct processo * processo );
void adicionar_fila_video( struct processo * processo );
void adicionar_fila_impressora( struct processo * processo );
void proximoFila_apto();
void proximoFila_hd();
void proximoFila_video();
void proximoFila_impressora();

//////////////////////////////////////////////
//                GLOBAIS                   //
//////////////////////////////////////////////
static struct processo* criacao = NULL;

//Executando
static struct processo* executando = NULL;
static struct item_fila* executando_hd = NULL;
static struct item_fila* executando_video = NULL;
static struct item_fila* executando_impressora = NULL;

//Contadores
static int criacao_ct = 0;
static int fila_apto = 0;
static int fila_hd = 0;
static int fila_video = 0;
static int fila_impressora = 0;

//Primeiro processo
struct processo* primeiroProcesso = NULL;
struct item_fila* primeiro_fila_apto =NULL;
struct item_fila* primeiro_fila_hd =NULL;
struct item_fila* primeiro_fila_video =NULL;
struct item_fila* primeiro_fila_impressora =NULL;
