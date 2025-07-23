///////////////////////////// +nan & -nan ///////////////////////////////////
//Yasmim Macêdo Gonçalves - 562188 , Gabriel Cavalcante dos Santos - 562431//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARAFINA 50
#define MAX_LEASH 25
#define MAX_QUILHA 10
#define MAX_DECK 5

typedef enum{ //Tipo de produtos
    parafina,
    leash,
    quilha,
    deck,
    cont
}tipo_produto;

typedef struct PRODUTO{ //Lista duplamente encadeada ordenada
    int codigo;
    tipo_produto tipo;
    char desc[30];
    float valor;
    int tam_lista_ordenada;
    struct PRODUTO * prox;
    struct PRODUTO * ant;
}produto;

typedef struct SELECIONADO{ //Lista encadeada
    int codigo;
    struct SELECIONADO * prox;
}produto_selecionado_caixa;

typedef struct CAIXA{ //Caixa com a lista encadeada
    int codigo_caixa;
    tipo_produto tipo;
    produto_selecionado_caixa * item;
    int quantidade;
    struct CAIXA * prox;
}caixa;

typedef struct PILHA{ //Pilha a ser inserida
    caixa * topo;
    int tam;
}pilha_estoque;

typedef struct VENDA{
    int codigo;
    tipo_produto tipo;
    char desc[30];
    float valor;
    // DADOS DO PRODUTO VENDIDO?
    char nome_cliente[50];
    int cpf;
    int cep;
    char nome_rua[100];
    int numero_residencia;
    char desc_loc[100];
    struct VENDA *prox;
}produto_venda;

//Globais
produto * inicio = NULL;
produto * fim = NULL;

pilha_estoque *p_parafina;
pilha_estoque *p_leash;
pilha_estoque *p_quilha;
pilha_estoque *p_deck;

produto_venda * inicio_fila = NULL;
produto_venda * fim_fila = NULL;
int tam_lista_ordenada = 0;

int tam = 0;
pilha_estoque * pilha[cont];

//Determina o limite para cada produto com eles definidos
int calcular_limite(tipo_produto tipo){
    int limite;
    switch (tipo)
    {
    case parafina: limite = MAX_PARAFINA; break;
    case leash: limite = MAX_LEASH; break;
    case quilha: limite = MAX_QUILHA; break;
    case deck: limite = MAX_DECK; break;
    default:
        printf("ERROR, PRODUTO INVALIDO! TENTE NOVAMENTE...");
        limite = 0;
        break;
    }
    return limite;    
}

//Lista ordenada por preço
void add_lista_ordenada(int codigo, tipo_produto tipo, char * desc, float valor){
    produto * novo = malloc(sizeof(produto));
    novo->codigo = codigo;
    novo->tipo = tipo;
    strncpy(novo->desc, desc, sizeof(novo->desc));
    novo->desc[sizeof(novo->desc)-1] = '\0';
    novo->valor = valor;
    novo->prox = NULL;
    novo->ant = NULL;

    if(inicio == NULL){
        inicio = novo;
        fim = novo;
    }else{
        if(valor < inicio->valor){
            inicio->ant = novo;
            novo->prox = inicio;
            inicio = novo;
        }else if(valor > fim->valor){
            fim->prox = novo;
            novo->ant = fim;
            fim = novo;
        }else{
            produto * aux = inicio;
            while(aux->valor <= novo->valor){
                aux = aux->prox;
            }
            novo->prox = aux;
            novo->ant = aux->ant;
            aux->ant->prox = novo;
            aux->ant = novo;
        }
    }
    tam_lista_ordenada++;
}

//Cria caixa nova
caixa * criar_caixa(int codigo_caixa, tipo_produto tipo, produto_selecionado_caixa item, int quantidade){
    caixa* nova = malloc(sizeof(caixa)); 
    nova->codigo_caixa = codigo_caixa;  
    nova->tipo = tipo;      
    nova->item = NULL;     
    nova->quantidade = 0;
    nova->prox = NULL;    
    return nova; 
}

//Adiciona o produto na caixa
void add_produto_caixa(caixa * caixa, int codigo){
    produto_selecionado_caixa * novo_produto = malloc(sizeof(produto_selecionado_caixa));
    novo_produto->codigo = codigo; 
    novo_produto->prox = caixa->item;
    caixa->item = novo_produto;
    caixa->quantidade++; 
}

//Cria uma pilha
pilha_estoque * criar_pilha(){
    pilha_estoque *p = malloc(sizeof(pilha_estoque));
    p->topo = NULL;
    p->tam = 0;
    return p;
}

//Adiciona a caixa ao estoque
void add_caixa_estoque(int codigo, tipo_produto tipo, int limite, int codigo_caixa, produto_selecionado_caixa item, int quantidade){
    pilha_estoque * pilha_tipo = pilha[tipo];
    calcular_limite(limite);

    if(pilha_tipo->topo == NULL || pilha_tipo->topo->quantidade >= limite){
        caixa * nova = criar_caixa(codigo_caixa, tipo, item, quantidade);
        nova->prox = pilha_tipo->topo;
        pilha_tipo->topo = nova;
        pilha_tipo->tam++;
    }

    add_produto_caixa(pilha_tipo->topo, codigo);
}

//Imprime os produtos dentro de uma caixa
void imprimir_lista_ordenada(){
    produto* p = inicio;
    if(p == NULL){
        printf("Lista ordenada vazia!\n");
        return;
    }
    printf("\n==== LISTA ORDENADA DE PRODUTOS ====\n");
    while(p != NULL){
        printf("Codigo: %d ; Tipo: %d ; Desc: %s ; Valor: R$ %.2f\n", p->codigo, p->tipo, p->desc, p->valor);
        p = p->prox;
    }
}

//Imprime a pilha dos produtos
void imprimir_pilha(pilha_estoque* p){
    caixa* aux = p->topo;
    if(aux == NULL){
        printf("Pilha vazia!\n");
        return;
    }
    printf("\n=== ESTOQUE TIPO %d ===\n", aux->tipo);
    while(aux != NULL){
        printf("Caixa Codigo: %d\nTipo: %d\nQuantidade de produtos: %d\n", aux->codigo_caixa, aux->tipo, aux->quantidade);

        printf("codigos: ");
        produto_selecionado_caixa* prod = aux->item;
        if(prod == NULL){
            printf("Nenhum produto\n");
        } else {
            while(prod != NULL){
                printf("%d ", prod->codigo);
                prod = prod->prox;
            }
            printf("\n");
        }
        aux = aux->prox;
    }
}

//Mostrar os produtos por categoria que se encontram no intervalo de determinado valor
void ver_produto(tipo_produto tipo, float lim_inf, float lim_sup){

    produto * p = inicio;
    if(p == NULL){
        printf("Lista ordenada vazia!\n");
        return;
    }
    int cont;
    printf("\n==========Produtos no intervalo de R$%.2f A R$%.2f:==========\n", lim_inf, lim_sup);

    while(p != NULL){
        if(p->tipo == tipo){

            if(p->valor <= lim_sup && p->valor >= lim_inf){

                printf("Codigo: %d ; Tipo: %d ; Desc: %s ; Valor: R$ %.2f\n", p->codigo, p->tipo, p->desc, p->valor);
                cont++;
            }
        }
        p = p->prox;
    }

    if(cont == 0){
        printf("Nenhum produto encontrado nesse intervalo.\n");
    }
}

//Imprimir o produto que foi vendido
void imprimir_produto_vendido(produto* p){
    printf("\n");
    printf("=============Produto vendido=============\n");
    printf("Codigo: %d ; Tipo: %d ; Desc: %s ; Valor: R$ %.2f\n", p->codigo, p->tipo, p->desc, p->valor);
}

//Remove o produto vendido da lista ordenada
produto * remover_lista_ordenada(int codigo) {
    produto *p = inicio;

    while (p != NULL) {
        if (p->codigo == codigo) {
            // Caso 1: Remoção do primeiro nó
            if (p->ant == NULL) {
                inicio = p->prox;
                if (inicio != NULL) {  // Verifica se a lista não ficou vazia
                    inicio->ant = NULL;
                } else {
                    fim = NULL;  // Lista esvaziada
                }
            }
            // Caso 2: Remoção do último nó
            else if (p->prox == NULL) {
                fim = p->ant;
                fim->prox = NULL;
            }
            // Caso 3: Remoção de nó no meio
            else {
                p->ant->prox = p->prox;
                p->prox->ant = p->ant;
            }

            imprimir_produto_vendido(p);
            int tipo = p->tipo;
            //free(p);
            tam--;
            return p;
        }
        p = p->prox;
    }

    return -1;  // Produto não encontrado
}


void push_caixa(pilha_estoque *p, caixa *nova_caixa) {
    if (p == NULL || nova_caixa == NULL) {
        printf("Erro: Pilha ou caixa inválida para push.\n");
        return;
    }
    nova_caixa->prox = p->topo;
    p->topo = nova_caixa;
    p->tam++;
}

//caixa *aux = pop_caixa(CAIXA);
//push_caixa(outra, AUX);

caixa *pop_caixa(pilha_estoque *p) {
    if (p == NULL || p->topo == NULL) {
        // printf("Erro: Pilha vazia ou inválida para pop.\n"); // Pode ser um comportamento esperado
        return NULL;
    }
    caixa *caixa_desempilhada = p->topo;
    p->topo = caixa_desempilhada->prox;
    caixa_desempilhada->prox = NULL; // Importante para isolar a caixa
    p->tam--;
    return caixa_desempilhada;
}

produto_selecionado_caixa * venda_produto(int codigo) {
    produto *p = remover_lista_ordenada(codigo);

    if (p->tipo == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }

    // Acessa a pilha do tipo correspondente
    pilha_estoque *original_pilha = pilha[p->tipo];

    if (original_pilha == NULL || original_pilha->topo == NULL) {
        printf("A pilha esta vazia.\n");
        return;
    }

    // Cria pilha auxiliar
    pilha_estoque *aux_pilha = criar_pilha();
    caixa *caixa_com_produto = NULL;
    int produto_encontrado = 0;
    
    while (original_pilha->topo != NULL && !produto_encontrado) {
        // Remove caixa do topo da pilha original
        caixa *caixa_atual = pop_caixa(original_pilha);
        if (caixa_atual == NULL) break;

        // Procura o produto na caixa atual
        produto_selecionado_caixa *produto_atual = caixa_atual->item;
        produto_selecionado_caixa *produto_ant = NULL;

        while (produto_atual != NULL) {
            if (produto_atual->codigo == codigo) {
                // Produto encontrado
                produto_encontrado = 1;
                caixa_com_produto = caixa_atual;

                // Remove o produto da caixa
                if (produto_ant == NULL) {
                    caixa_atual->item = produto_atual->prox;
                } else {
                    produto_ant->prox = produto_atual->prox;
                }

                //free(produto_atual);
                caixa_atual->quantidade--;
                return p;
            }

            produto_ant = produto_atual;
            produto_atual = produto_atual->prox;
        }

        // Coloca a caixa na pilha auxiliar
        push_caixa(aux_pilha, caixa_atual);
    }
    

    if (!produto_encontrado) {
        printf("Produto nao encontrado no estoque.\n");
        return NULL;

    }

    // Devolve as caixas da pilha auxiliar para a original
    while (aux_pilha->topo != NULL) {
        caixa *c = pop_caixa(aux_pilha);
        push_caixa(original_pilha, c);
    }

    free(aux_pilha);
    return p;
}


//add produto para venda na fila de produtos para entrega
void add_produto_venda(int codigo, tipo_produto tipo, char * desc, float valor, char * nome_cliente, int cpf, int cep, char * nome_rua, int numero_residencia, char * desc_loc){
    produto_venda * nova = malloc(sizeof(produto_venda));
    
    nova->codigo = codigo;
    nova->tipo = tipo;
    strncpy(nova->desc, desc, sizeof(nova->desc));
    nova->desc[sizeof(nova->desc)-1] = '\0';
    nova->valor = valor;
    strncpy(nova->nome_cliente, nome_cliente, sizeof(nova->nome_cliente));
    nova->nome_cliente[sizeof(nova->nome_cliente)-1] = '\0';
    nova->cpf = cpf;
    nova->cep = cep;
    strncpy(nova->nome_rua, nome_rua, sizeof(nova->nome_rua));
    nova->nome_rua[sizeof(nova->nome_rua)-1] = '\0';
    nova->numero_residencia = numero_residencia;
    strncpy(nova->desc_loc, desc_loc, sizeof(nova->desc_loc));
    nova->desc_loc[sizeof(nova->desc_loc)-1] = '\0';
    nova->prox = NULL;

    if(inicio_fila == NULL){
        inicio_fila = nova;
        fim_fila = nova;
    } else{
        fim_fila->prox = nova;
        fim_fila = nova;
    }

}

//imrpimir a fila de produtos para a venda
void imprimir_fila_produto_venda(){
    produto_venda*p = inicio_fila;
    printf("\n");
    printf("=============Fila de produtos a serem entregues=============\n");
    printf("\n");

    while (p != NULL){   
        printf("Codigo: %d ; Tipo: %d ; Desc: %s ; Valor: R$ %.2f ||  \n", p->codigo, p->tipo, p->desc, p->valor);
        printf("Cliente: %s || CPF: %d || CEP: %d || Endereco: rua %s, numero %d || Ponto de referencia: %s\n", p->nome_cliente, p->cpf, p->cep, p->nome_rua, p->numero_residencia, p->desc_loc);

        p = p->prox;
        printf("\n");
    }
}

//remover produto para venda na fila de produtos para entrega
void remover_produto_venda(){
    
    if(inicio_fila == NULL){
        printf("Fila vazia!");
        return;
    }
        
    produto_venda* p = inicio_fila;
    inicio_fila = p->prox;

    if (inicio_fila == NULL) {
        fim_fila = NULL;
    }

    printf("\n");
    printf("=============Produto saindo para a entrega=============\n");
    printf("Codigo: %d ; Tipo: %d ; Desc: %s ; Valor: R$ %.2f\n", p->codigo, p->tipo, p->desc, p->valor);
    printf("Cliente: %s || CPF: %d || CEP: %d || Endereco: rua %s, numero: %d || Ponto de referencia: %s\n", p->nome_cliente, p->cpf, p->cep, p->nome_rua, p->numero_residencia, p->desc_loc);
    free(p);
}

int main(){

    //Os produtos sendo adcionados na lista duplamente encadeada
    add_lista_ordenada(1, quilha, "Pukas", 400.00);
    add_lista_ordenada(2, deck, "Sympl Supply Co.", 170.00);
    add_lista_ordenada(3, parafina, "GreenFix", 30.00);
    add_lista_ordenada(4, leash, "Rip Curl", 125.00);
    add_lista_ordenada(5, quilha, "Al Merrick", 500.00);
    add_lista_ordenada(6, deck, "AstroDeck", 160.00);
    add_lista_ordenada(7, parafina, "Bubble Gum", 27.00);
    add_lista_ordenada(8, leash, "Balin", 110.00);
    add_lista_ordenada(9, quilha, "Chilli", 430.00);
    add_lista_ordenada(10, deck, "Modom", 200.00);
    add_lista_ordenada(11, parafina, "Famous Wax", 23.00);
    add_lista_ordenada(12, leash, "Ocean & Earth", 30.00);
    add_lista_ordenada(13, quilha, "HaydenShapes", 470.00);
    add_lista_ordenada(14, deck, "Traction Pads", 150.00);
    add_lista_ordenada(15, parafina, "Sticky Bumps", 25.50);
    add_lista_ordenada(16, leash, "Creatures", 120.00);
    add_lista_ordenada(17, quilha, "FCS", 450.00);
    add_lista_ordenada(18, deck, "Ocean & Earth", 110.00);
    add_lista_ordenada(19, parafina, "Fu-Wax", 28.00);
    add_lista_ordenada(20, leash, "Dakine", 115.00);
    add_lista_ordenada(21, quilha, "Futures Fins", 410.00);
    add_lista_ordenada(22, deck, "Gorilla Grip", 125.00);
    add_lista_ordenada(23, parafina, "Mrs. Palmers", 26.50);
    add_lista_ordenada(24, leash, "O'Neill", 105.00);
    add_lista_ordenada(25, quilha, "Shapers", 420.00);
    add_lista_ordenada(26, deck, "Slater Designs", 210.00);
    add_lista_ordenada(27, parafina, "Sex Wax", 24.00);
    add_lista_ordenada(28, leash, "Pro-Lite", 130.00);
    add_lista_ordenada(29, quilha, "Pukas", 400.00);
    add_lista_ordenada(30, deck, "Sympl Supply Co.", 170.00);

    // Criar caixas para cada tipo de produto
    caixa *caixa_parafina = criar_caixa(1, parafina, (produto_selecionado_caixa){0}, 0);
    caixa *caixa_leash = criar_caixa(1, leash, (produto_selecionado_caixa){0}, 0);
    caixa *caixa_quilha = criar_caixa(1, quilha, (produto_selecionado_caixa){0}, 0);
    caixa *caixa_deck = criar_caixa(1, deck, (produto_selecionado_caixa){0}, 0);

    // Adicionar os produtos nas respectivas caixas
    add_produto_caixa(caixa_parafina, 3);
    add_produto_caixa(caixa_parafina, 7);
    add_produto_caixa(caixa_parafina, 11);
    add_produto_caixa(caixa_parafina, 15);
    add_produto_caixa(caixa_parafina, 19);
    add_produto_caixa(caixa_parafina, 23);
    add_produto_caixa(caixa_parafina, 27);

    add_produto_caixa(caixa_leash, 4);
    add_produto_caixa(caixa_leash, 8);
    add_produto_caixa(caixa_leash, 12);
    add_produto_caixa(caixa_leash, 16);
    add_produto_caixa(caixa_leash, 20);
    add_produto_caixa(caixa_leash, 24);
    add_produto_caixa(caixa_leash, 28);

    add_produto_caixa(caixa_quilha, 1);
    add_produto_caixa(caixa_quilha, 5);
    add_produto_caixa(caixa_quilha, 9);
    add_produto_caixa(caixa_quilha, 13);
    add_produto_caixa(caixa_quilha, 17);
    add_produto_caixa(caixa_quilha, 21);
    add_produto_caixa(caixa_quilha, 25);
    add_produto_caixa(caixa_quilha, 29);

    add_produto_caixa(caixa_deck, 2);
    add_produto_caixa(caixa_deck, 6);
    add_produto_caixa(caixa_deck, 10);
    add_produto_caixa(caixa_deck, 14);
    add_produto_caixa(caixa_deck, 18);
    add_produto_caixa(caixa_deck, 22);
    add_produto_caixa(caixa_deck, 26);
    add_produto_caixa(caixa_deck, 30);

    // caixas nas pilhas
    p_parafina = criar_pilha();
    p_leash = criar_pilha();
    p_quilha = criar_pilha();
    p_deck = criar_pilha();

    pilha[parafina] = p_parafina;
    pilha[leash] = p_leash;
    pilha[quilha] = p_quilha;
    pilha[deck] = p_deck;


    pilha[parafina]->topo = caixa_parafina;
    pilha[parafina]->tam = caixa_parafina->quantidade;

    pilha[leash]->topo = caixa_leash;
    pilha[leash]->tam = caixa_leash->quantidade;

    pilha[quilha]->topo = caixa_quilha;
    pilha[quilha]->tam = caixa_quilha->quantidade;

    pilha[deck]->topo = caixa_deck;
    pilha[deck]->tam = caixa_deck->quantidade;


    // Imprimir a lista ordenada
    imprimir_lista_ordenada();

    // Imprimir as pilhas
    for(int i = 0; i < cont; i++){
        imprimir_pilha(pilha[i]);
    }

    //Mostra o "cardápio" de produtos
    ver_produto(parafina, 25.00, 500.00);

    produto * produto_vendido = venda_produto(4);
    add_produto_venda(produto_vendido->codigo, produto_vendido->tipo, produto_vendido->desc, produto_vendido->valor, "Gabriel Cavalcante", 400289, 829172, "JUscelino kubtchek", 2, "perto da ufc");
    free(produto_vendido);
    
    produto* produto_vendido = venda_produto(24);
    add_produto_venda(produto_vendido->codigo, produto_vendido->tipo, produto_vendido->desc, produto_vendido->valor, "Yasmin Macedo", 672229, 983321, "Avenida santiago", 45, "bloco 302");
    free(produto_vendido);

    produto* produto_vendido = venda_produto(12);
    add_produto_venda(produto_vendido->codigo, produto_vendido->tipo, produto_vendido->desc, produto_vendido->valor, "Jenifer lopez", 1223456, 544211, "rosalina", 7, "proximo a guarita");
    free(produto_vendido);

    produto* produto_vendido = venda_produto(27);
    add_produto_venda(produto_vendido->codigo, produto_vendido->tipo, produto_vendido->desc, produto_vendido->valor, "Pit Bull", 4454289, 824572, "Dolores silva", 55, "kitnet");
    free(produto_vendido);


    imprimir_fila_produto_venda();

    remover_produto_venda();

    imprimir_fila_produto_venda();
    

    return 0;
}
