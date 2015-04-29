

#include "sfp.h"

int main( int argc, char *argv[] ){

	srand ( time(NULL) );
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	int i;
	int sorteio;
	int ct_maxCiclos = 0;
	int destruidos = 0;
	int totalProcessos = atoi( argv[1] );
	int segundos = atoi( argv[2] );
	struct processo* anterior = NULL;
	struct processo* processoAtual = NULL;
	struct processo* primeiroProcesso = NULL;

	//LOOP PRINCIPAL
	while( destruidos < totalProcessos ){
		//EXECUTANDO

		if( executando != NULL ){
			//há um processo sendo executado

			executando->ciclos_executados++;
			ct_maxCiclos++;

			if( executando->ciclos_executados == executando->totalCiclos ){
				//Se o processo já atingiu o máximo de ciclos

				executando->estado = ESTADO_DESTRUICAO;
				executando->ct_estado[ ESTADO_DESTRUICAO ]++;
				destruidos++;
				executando = NULL;
				ct_maxCiclos = 0;
	
			}else{

				sorteio = sortearEntre( 1, 100 );
				if( sorteio == 1){
					//Sorteio com 1% de chance de solicitar um recurso de E/S
					ct_maxCiclos = 0;

					switch( sortearEntre( 0, 3 ) ){
						case SOLICITAR_HD          :
							adicionar_fila_hd( executando );
							executando = NULL;
							break;
						case SOLICITAR_VIDEO       :
							adicionar_fila_video( executando );
							executando = NULL;
							break;
						case SOLICITAR_IMPRESSORA  :
							adicionar_fila_impressora( executando );
							executando = NULL;
							break;
					}

				}
			}


		}else{
			//nenhum processo está em execução, há alguém na fila de aptos?
			if( primeiro_fila_apto != NULL ){
				executando = primeiro_fila_apto->atual;
				executando->estado = ESTADO_EXECUCAO;
				executando->ct_estado[ ESTADO_EXECUCAO ]++;
				ct_maxCiclos = 0;

				proximoFila_apto();
			}
		}


		/*********************** RECURSOS ***********************/

		//HD
		if( executando_hd != NULL ){
			//há um processo utilizando o recurso hd
			executando_hd->cls_rec_exec++;

			if( executando_hd->cls_rec_exec == executando_hd->cls_rec_total ){
				//Processo terminou de utilizar o recurso, volta para a fila de aptos
				adicionar_fila_apto( executando_hd->atual );
				executando_hd = NULL;

			}
		}else{
			//nenhum processo está em execução, há alguém na fila do hd
			if( primeiro_fila_hd != NULL ){
				executando_hd = primeiro_fila_hd;
				executando_hd->atual->estado = ESTADO_BLOQUEADO;
				executando_hd->atual->ct_estado[ ESTADO_BLOQUEADO ]++;

				proximoFila_hd();
			}
		}

		//VIDEO
		if( executando_video != NULL ){
			//há um processo utilizando o recurso video
			executando_video->cls_rec_exec++;

			if( executando_video->cls_rec_exec == executando_video->cls_rec_total ){
				//Processo terminou de utilizar o recurso, volta para a fila de aptos
				adicionar_fila_apto( executando_video->atual );
				executando_video = NULL;

			}
		}else{
			//nenhum processo está em execução, há alguém na fila de video?
			if( primeiro_fila_video != NULL ){
				executando_video = primeiro_fila_video;
				executando_video->atual->estado = ESTADO_BLOQUEADO;
				executando_video->atual->ct_estado[ ESTADO_BLOQUEADO ]++;

				proximoFila_video();
			}
		}

		//IMPRESSORA
		if( executando_impressora != NULL ){
			//há um processo utilizando o recurso impressora
			executando_impressora->cls_rec_exec++;

			if( executando_impressora->cls_rec_exec == executando_impressora->cls_rec_total ){
				//Processo terminou de utilizar o recurso, volta para a fila de aptos
				adicionar_fila_apto( executando_impressora->atual );
				executando_impressora = NULL;

			}
		}else{
			//nenhum processo está em execução, há alguém na fila da impressora?
			if( primeiro_fila_impressora != NULL ){
				executando_impressora = primeiro_fila_impressora;
				executando_impressora->atual->estado = ESTADO_BLOQUEADO;
				executando_impressora->atual->ct_estado[ ESTADO_BLOQUEADO ]++;

				proximoFila_impressora();
			}
		}
		/***************************************************************************/
		if( criacao_ct > 0 && criacao != NULL ){
			//Existe um processo no estado de criação, e já passou um ciclo
			adicionar_fila_apto(criacao);
			criacao = NULL;

		}

		if( pIdCounter < totalProcessos ){
			sorteio = sortearEntre(1,100);
			//printf("%d\n",sorteio );

			// 20% de chance de criar um novo processo
			if( sorteio > 0 && sorteio <= 20 ){
				pIdCounter++;
				criarProcesso( pIdCounter );
				criacao_ct = 0;
			}
		}


		if( ct_maxCiclos == MAX_CICLOS_EXEC ){
			ct_maxCiclos = 0;
			adicionar_fila_apto( executando );
			executando = NULL;
			retirados_exec++;
		}

		system("cls");

		mostrarLog();
		sleep(segundos);

		//Incrementar contadores
		if( destruidos < pIdCounter){
			criacao_ct++;
			ciclos++;
		}
	}

	system("cls");
	mostrarRelatorio();
	getchar();


	return 0;
}
////////////////////////////////////////////////////////////////////////////
/*
 * Sorteia um número entre um intervalo, que é definido por min e max
 */
int sortearEntre( int min, int max){
	int n;
	n = rand() % (max - min) + min;
	//printf("%d ",n );
	return n;
}
////////////////////////////////////////////////////////////////////////////
/*
 * Aloca o espaço para uma struct processo
 * -seta o pid que ela recebe como parâmetro
 * -seta o número total de ciclos
 */
void criarProcesso(int pid){

	static struct processo* anterior = NULL;
	static struct processo* processoAtual = NULL;

	int i;
	//printf(" Processo criado -pid:%d\n",pid );

	//Criar processo
	processoAtual = malloc( sizeof(struct processo) );

	processoAtual->pid = pid;
	processoAtual->estado = ESTADO_CRIACAO;
	processoAtual->cls_filaApto = 0;

	for( i = 0; i < 5; i++ ){
		//Zerar contadores de estado
		processoAtual->ct_estado[ i ] = 0;
	}

	processoAtual->ct_estado[ ESTADO_CRIACAO ]++;
	processoAtual->totalCiclos = sortearEntre( MIN_CICLOS_PROCESSO,MAX_CICLOS_PROCESSO); 
	processoAtual->ciclos_executados = 0;

	total_medio += processoAtual->totalCiclos;

	if( pid == 1 ){
		primeiroProcesso = processoAtual;
	}

	//Se acabei de criar, então é o último, não há próximo
	processoAtual->proximo = NULL;

	if( anterior != NULL ){
		/*Caso haja algum processo anterior
		 * o processo atual aponta para ele,
		 * e o anterior aponta pra o processo atual
		 */
		processoAtual->anterior = anterior;

		anterior->proximo = processoAtual;
	}

	// Ao final este processo será o processo anterior
	anterior = processoAtual;
	criacao = processoAtual;
}
////////////////////////////////////////////////////////////////////////////
#define ESPACO( esp, len) floor( ( esp ) / 2 +  ( len ) / 2 )
void mostrarLog(){
	int espaco;
	int color_code = 0;
	char c_estado[15];
	char c_pid[15];
	char aux[15];
	struct processo* processo = NULL;
	processo = primeiroProcesso;

	set_color( COR_TITULO );
	printf("|  RECURSOS  | PID |  CICLOS  | FILA |\n");
	set_color( COR_DEFAULT );

	/*********************************** HD ************************************/
	set_color(71);
	if( executando_hd != NULL ){
		printf("|     HD     |  %d |   %d-%d  |  %d  |\n",executando_hd->atual->pid, executando_hd->cls_rec_exec, executando_hd->cls_rec_total,fila_hd);
	} else{
		printf("|     HD     | xxx |    xxx   |  xxx |\n");
	}
	/********************************* VIDEO ***********************************/

	if( executando_video != NULL ){
		printf("|   VIDEO    |  %d |   %d-%d  |  %d  |\n",executando_video->atual->pid, executando_video->cls_rec_exec, executando_video->cls_rec_total,fila_video);
	} else{
		printf("|   VIDEO    | xxx |    xxx   |  xxx |\n");
	}
	/******************************* IMPRESSORA ********************************/

	if( executando_impressora != NULL ){
		printf("| IMPRESSORA |  %d |   %d-%d  |  %d  |\n",executando_impressora->atual->pid, executando_impressora->cls_rec_exec, executando_impressora->cls_rec_total,fila_impressora);
	} else{
		printf("| IMPRESSORA | xxx |    xxx   |  xxx |\n");
	}
	set_color(15);

	/****************************************************************************/
	set_color( COR_VERMELHO );
	printf("\n\n______________________________________________________________\n");
	set_color( COR_TITULO );
	printf("| PID |     ESTADO     | CICLOS EXECUTADOS | TOTAL DE CICLOS |\n");
	set_color( COR_DEFAULT );

	do{
		if( processo == NULL)
			return;

		//Centralizar PID
		snprintf( aux, sizeof( aux ), "%d", processo->pid);
		espaco = ESPACO( 5,strlen( aux) );
		snprintf( c_pid, sizeof(c_pid), "%*s%*s",espaco ,aux,espaco,"" );

		memset( c_estado, 0, sizeof(c_estado) );
		switch( processo->estado ){

			case ESTADO_CRIACAO:
				snprintf( c_estado, sizeof( c_estado ), "CRIACAO" );
				color_code = COR_CRIACAO;
				break;
			case ESTADO_APTO:
				//Caso seja apto, incrementamos o contador cls_filaApto, para termos o tempo médio na fila
				processo->cls_filaApto++;

				snprintf( c_estado, sizeof( c_estado ), "APTO" );
				color_code = COR_APTO;
				break;
			case ESTADO_EXECUCAO:
				snprintf( c_estado, sizeof( c_estado ), "EXECUCAO" );
				color_code = COR_EXECUCAO;
				break;
			case ESTADO_BLOQUEADO:
				snprintf( c_estado, sizeof( c_estado ), "BLOQUEADO" );
				color_code = COR_BLOQUEADO;
				break;
			case ESTADO_DESTRUICAO:
				snprintf( c_estado, sizeof( c_estado ), "DESTRUICAO" );
				color_code = COR_DESTRUICAO;
				break;
		}

		int len_str = strlen( c_estado );
		int left_spc = floor(   15 / 2 + len_str / 2);
		

		snprintf( aux, sizeof(aux), c_estado );

		snprintf( c_estado, sizeof(c_estado), "%*s%*s",left_spc,aux,left_spc,"");

		set_color( COR_FUNDO_VERMELHO );

		printf("|%s", c_pid );
		printf("|");

		set_color( color_code );
		printf(" %s ", c_estado );
		set_color( COR_FUNDO_VERMELHO );

		printf("|     %3d          ", processo->ciclos_executados );
		printf("| %3d           |\n", processo->totalCiclos);
		set_color( COR_DEFAULT);



		// printf("|%s| %s |     %3d          | %3d           |\n",
		// 			c_pid,
		// 			c_estado,
		// 			processo->ciclos_executados,
		// 			processo->totalCiclos);

		processo = processo->proximo;




	}while( processo != NULL );

}
////////////////////////////////////////////////////////////////////////////
	/*
	1. O número total de processos que foram criados;
	2. O tempo total (medido em ciclos de CPU) para atender a todos os processos;
	3. O tempo total médio (número de ciclos) de execução de um processo;
	4. A quantidade de processos que passou por cada estado (se um processo passa mais de uma vez
	pelo mesmo estado, este conta uma só vez);
	5. O tempo médio de espera de um processo na fila de aptos;
	6. A quantidade de vezes que processos tiveram de ser retirados do estado de execução e devolvidos
	para a fila de aptos.
	*/
void mostrarRelatorio(){
	total_medio = total_medio / pIdCounter;
	float total_medio_fAptos = 0;

	int i;
	int ct_estados[ 5 ] = { 0, 0, 0, 0, 0 };
	struct processo* processo = NULL;
	processo = primeiroProcesso;


	do{
		if( processo == NULL)
			return;

		for( i = 0; i < 5; i++ ){

			ct_estados[i] += processo->ct_estado[i] >= 1 ? 1 : 0;
		}

		total_medio_fAptos += processo->cls_filaApto;

		processo = processo->proximo;

	}while( processo != NULL );

	total_medio_fAptos = total_medio_fAptos / pIdCounter;

	printf(
		"1.PROCESSOS CRIADOS :          %d \n" 
		"2.TEMPO TOTAL :                %d CICLOS \n"
		"3.TEMPO MEDIO DE EXECUCAO :    %.1f CICLOS \n"
		"4.QUANTIDADE DE PROCESSOS QUE PASSOU POR CADA ESTADO: \n"
		"  4.1 CRIACAO :    %d \n"
		"  4.2 APTO :       %d \n"
		"  4.3 EXECUCAO :   %d \n"
		"  4.4 BLOQUEADO :  %d \n"
		"  4.5 DESTRUICAO : %d \n"

		"5.TEMPO MEDIO NA FILA DE APTOS :    %.1f CICLOS \n"
		"6.RETIRADOS DO ESTADO DE EXECUCAO : %d \n",
		pIdCounter,
		ciclos,
		total_medio,
		ct_estados[ ESTADO_CRIACAO ],
		ct_estados[ ESTADO_APTO ],
		ct_estados[ ESTADO_EXECUCAO ],
		ct_estados[ ESTADO_BLOQUEADO ],
		ct_estados[ ESTADO_DESTRUICAO ],
		total_medio_fAptos,
		retirados_exec
	);


}
////////////////////////////////////////////////////////////////////////////
void mostrarLog_aptos(){
	struct item_fila * fila_aux = NULL;
	struct item_fila * prox = NULL;

	//log fila de aptos
	printf("\n\n FILA DE APTOS\n\n");

	fila_aux = primeiro_fila_apto;

	if( fila_aux == NULL ){
		return;
	}
	do{

		printf("%d - ", fila_aux->atual->pid);

		
		prox = fila_aux->proximo ;

		fila_aux = prox != NULL ? prox : NULL;



	}while( fila_aux != NULL);
}
////////////////////////////////////////////////////////////////////////////
/*
 * Aloca uma estrutura do tipo item_fila,
 * seta o processo no campo atual, 
 * se não for o primeiro, faz o ultimo que entrou na fila apontar para o que está entrando
 */
void adicionar_fila_apto( struct processo * processo ){
	static struct item_fila * anterior_fila_apto = NULL; //o último processo a entrar na fila
	struct item_fila * fila_aux = NULL; //usada para alocar os processos seguintes

	processo->estado = ESTADO_APTO;
	processo->ct_estado[ ESTADO_APTO ]++;

	if( fila_apto == 0 ){
		//fila vazia

		anterior_fila_apto = malloc( sizeof( struct item_fila ) );
		anterior_fila_apto->atual = processo;
		anterior_fila_apto->proximo = NULL;
		primeiro_fila_apto = anterior_fila_apto;
		
	}else{

		fila_aux = malloc(  sizeof( struct item_fila ) );
		fila_aux->atual = processo;
		fila_aux->proximo = NULL;

		//o penúltimo aponta para o último
		anterior_fila_apto->proximo = fila_aux;

		//o último se torna o anterior para o próximo que entrar
		anterior_fila_apto = fila_aux;
	}
	
	fila_apto++;
}
////////////////////////////////////////////////////////////////////////////
void adicionar_fila_hd( struct processo * processo ){
	static struct item_fila * anterior_fila_hd = NULL; //o último processo a entrar na fila
	struct item_fila * fila_aux = NULL; //usada para alocar os processos seguintes

	processo->estado = ESTADO_BLOQUEADO;
	processo->ct_estado[ ESTADO_BLOQUEADO ]++;

	if( fila_hd == 0 ){
		//fila vazia

		anterior_fila_hd = malloc( sizeof( struct item_fila ) );
		anterior_fila_hd->atual = processo;
		anterior_fila_hd->proximo = NULL;
		anterior_fila_hd->cls_rec_total = sortearEntre( MIN_CICLOS_HD, MAX_CICLOS_HD );
		anterior_fila_hd->cls_rec_exec = 0;
		primeiro_fila_hd = anterior_fila_hd;
		
	}else{

		fila_aux = malloc(  sizeof( struct item_fila ) );
		fila_aux->atual = processo;
		fila_aux->proximo = NULL;

		fila_aux->cls_rec_total = sortearEntre( MIN_CICLOS_HD, MAX_CICLOS_HD );
		fila_aux->cls_rec_exec = 0;

		//o penúltimo aponta para o último
		anterior_fila_hd->proximo = fila_aux;

		//o último se torna o anterior para o próximo que entrar
		anterior_fila_hd = fila_aux;
	}
	
	fila_hd++;
}
////////////////////////////////////////////////////////////////////////////
void adicionar_fila_video( struct processo * processo ){
	static struct item_fila * anterior_fila_video = NULL; //o último processo a entrar na fila
	struct item_fila * fila_aux = NULL; //usada para alocar os processos seguintes

	processo->estado = ESTADO_BLOQUEADO;
	processo->ct_estado[ ESTADO_BLOQUEADO ]++;

	if( fila_video == 0 ){
		//fila vazia

		anterior_fila_video = malloc( sizeof( struct item_fila ) );
		anterior_fila_video->atual = processo;
		anterior_fila_video->proximo = NULL;
		anterior_fila_video->cls_rec_total = sortearEntre( MIN_CICLOS_VIDEO, MAX_CICLOS_VIDEO );
		anterior_fila_video->cls_rec_exec = 0;
		primeiro_fila_video = anterior_fila_video;
		
	}else{

		fila_aux = malloc(  sizeof( struct item_fila ) );
		fila_aux->atual = processo;
		fila_aux->proximo = NULL;

		fila_aux->cls_rec_total = sortearEntre( MIN_CICLOS_VIDEO, MAX_CICLOS_VIDEO );
		fila_aux->cls_rec_exec = 0;

		//o penúltimo aponta para o último
		anterior_fila_video->proximo = fila_aux;

		//o último se torna o anterior para o próximo que entrar
		anterior_fila_video = fila_aux;
	}
	
	fila_video++;
}
////////////////////////////////////////////////////////////////////////////
void adicionar_fila_impressora( struct processo * processo ){
	static struct item_fila * anterior_fila_impressora = NULL; //o último processo a entrar na fila
	struct item_fila * fila_aux = NULL; //usada para alocar os processos seguintes

	processo->estado = ESTADO_BLOQUEADO;
	processo->ct_estado[ ESTADO_BLOQUEADO ]++;

	if( fila_impressora == 0 ){
		//fila vazia

		anterior_fila_impressora = malloc( sizeof( struct item_fila ) );
		anterior_fila_impressora->atual = processo;
		anterior_fila_impressora->proximo = NULL;
		anterior_fila_impressora->cls_rec_total = sortearEntre( MIN_CICLOS_IMPRESSORA, MAX_CICLOS_IMPRESSORA );
		anterior_fila_impressora->cls_rec_exec = 0;
		primeiro_fila_impressora = anterior_fila_impressora;
		
	}else{

		fila_aux = malloc(  sizeof( struct item_fila ) );
		fila_aux->atual = processo;
		fila_aux->proximo = NULL;

		fila_aux->cls_rec_total = sortearEntre( MIN_CICLOS_IMPRESSORA, MAX_CICLOS_IMPRESSORA );
		fila_aux->cls_rec_exec = 0;

		//o penúltimo aponta para o último
		anterior_fila_impressora->proximo = fila_aux;

		//o último se torna o anterior para o próximo que entrar
		anterior_fila_impressora = fila_aux;
	}
	
	fila_impressora++;
}
////////////////////////////////////////////////////////////////////////////
void proximoFila_apto(){
	struct item_fila * proximo = NULL;

	if( primeiro_fila_apto->proximo == NULL ){
		//havia apenas um na fila

		primeiro_fila_apto = NULL;

	}else{


		proximo = primeiro_fila_apto->proximo; //guardar o endereço do proximo da fila
		//free( primeiro_fila_apto ); //free no item_fila atual

		primeiro_fila_apto = proximo; //colocar o proximo da fila como primeiro
	}
	fila_apto--;
}
////////////////////////////////////////////////////////////////////////////
void proximoFila_hd(){
	struct item_fila * proximo = NULL;

	if( primeiro_fila_hd->proximo == NULL ){
		//havia apenas um na fila
		primeiro_fila_hd = NULL;

	}else{

		proximo = primeiro_fila_hd->proximo; //guardar o endereço do proximo da fila
		//free( primeiro_fila_hd ); //free no item_fila atual

		primeiro_fila_hd = proximo; //colocar o proximo da fila como primeiro
	}
	fila_hd--;
}
////////////////////////////////////////////////////////////////////////////
void proximoFila_video(){
	struct item_fila * proximo = NULL;

	if( primeiro_fila_video->proximo == NULL ){
		//havia apenas um na fila
		primeiro_fila_video = NULL;

	}else{

		proximo = primeiro_fila_video->proximo; //guardar o endereço do proximo da fila
		//free( primeiro_fila_video ); //free no item_fila atual

		primeiro_fila_video = proximo; //colocar o proximo da fila como primeiro
	}
	fila_video--;
}
////////////////////////////////////////////////////////////////////////////
void proximoFila_impressora(){
	struct item_fila * proximo = NULL;

	if( primeiro_fila_impressora->proximo == NULL ){
		//havia apenas um na fila
		primeiro_fila_impressora = NULL;

	}else{

		proximo = primeiro_fila_impressora->proximo; //guardar o endereço do proximo da fila
		//free( primeiro_fila_impressora ); //free no item_fila atual

		primeiro_fila_impressora = proximo; //colocar o proximo da fila como primeiro
	}
	fila_impressora--;
}
////////////////////////////////////////////////////////////////////////////
void set_color( int color_code){
	SetConsoleTextAttribute(hConsole, color_code);
}
