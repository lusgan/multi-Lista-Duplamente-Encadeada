#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define NOME_ARQUIVO "arquivo.txt"

typedef struct  subnoLDDE
{
    int linha;
    int coluna;
    char palavra[100];
    struct subnoLDDE *proxima;
    struct subnoLDDE *anterior;
}Palavras; //struct que representa cada palavra contida no texto.

typedef struct noLDDE
{
    int num_da_linha;
    int num_de_palavras;
    Palavras *Primeira_palavra_da_linha;
    struct noLDDE *linhaCima;
    struct noLDDE *linhaBaixo;
}Linhas; // struct representando uma linha que contém várias structs Palavras.

typedef struct LDDE
{
    int numLinhas;
    Linhas *Primeira_linha;

}Multilista; // Struct que representa todo o texto

// a funcao abaixo cria e retorna um ponteiro para uma palavra
Palavras* criarSubNo(char *palavra, int linha, int col){
    Palavras *NovoNo = malloc(sizeof(Palavras));
    NovoNo->linha = linha;
    NovoNo->coluna = col;
    strcpy(NovoNo->palavra,palavra);
    NovoNo->proxima = NULL;

    return NovoNo;
}

// aloca o texto na multilista-LDDE
void Alocar_texto(Multilista *multilista){
    FILE *file;
    char line[1000]; // Tamanho maximo de cada linha do arquivo

    int lineCount = 1; // Contador de linhas

    multilista->Primeira_linha = malloc(sizeof(Linhas));
    multilista->Primeira_linha->linhaCima = NULL;



    // Abre o arquivo em modo de leitura
    file = fopen(NOME_ARQUIVO, "r");

    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return ;
    }

    Linhas *Linha_atual = multilista->Primeira_linha;

    // Le cada linha do arquivo
    while (fgets(line, sizeof(line), file) != NULL) {

        char *palavra = strtok(line," ");
        int col = 1;

        Linha_atual->Primeira_palavra_da_linha = criarSubNo(palavra,lineCount,col);
        Linha_atual->num_de_palavras = 1;

        Palavras *Palvra_atual = Linha_atual->Primeira_palavra_da_linha;

        Palvra_atual->anterior = NULL;


        while(palavra !=NULL){
            if(col>=2){
                Palvra_atual->proxima = criarSubNo(palavra,lineCount,col);
                Palvra_atual->proxima->anterior = Palvra_atual;
                Palvra_atual = Palvra_atual->proxima;
                Linha_atual->num_de_palavras++;
            }
            palavra = strtok(NULL," \n");
            col += strlen(Palvra_atual->palavra)+1;
        }

        lineCount++;
        Linha_atual->num_da_linha = lineCount;
        Linha_atual->linhaBaixo = malloc(sizeof(Linhas));
        Linha_atual = Linha_atual->linhaBaixo;

    }

    // Fecha o arquivo
    fclose(file);
    multilista->numLinhas = lineCount-1;

}

void mostrarLinha(Linhas *linha){

    Palavras *atual = linha->Primeira_palavra_da_linha;
    printf("Numero de palavras: %d\n",linha->num_de_palavras);
    while(atual!=NULL){

        printf("|%d.%d|%s ",atual->linha,atual->coluna,atual->palavra);
        atual = atual->proxima;

    }
    printf("\n");

}

void mostrar_Todas_AS_Linhas(Multilista *multilista){

    Linhas *atual = multilista->Primeira_linha;

    for(int i = 0; i<multilista->numLinhas;i++){
        mostrarLinha(atual);
        atual = atual->linhaBaixo;
        printf("\n");
    }


}

typedef struct{
    char palavra[100];
    int *linhas;
    int *colunas;
    int qtd_de_ocorrencias;

}Ocorrencias; // struc contendo todas as ocorrencias de uma palavra especifica

//a funcao abaixo retorna um ponteiro para a struct "Ocorrencias" que ocontem todas as ocorrencia de "palavra"
Ocorrencias* Ponteiro_Ocorrencias_da_palavra(char *palavra, Multilista *multilista){

    Ocorrencias *ocorrencias = malloc(sizeof(Ocorrencias));
    ocorrencias->qtd_de_ocorrencias = 0;
    ocorrencias->colunas = malloc(0);
    ocorrencias->linhas = malloc(0);
    strcpy(ocorrencias->palavra,palavra);

    Linhas *atual = multilista->Primeira_linha;

    int aux = 0; // vai ajudar a alocar na posiçao correta dos vetores, e tambem para realocar os vetores

    for(int i  = 0; i<multilista->numLinhas;i++){ //Percorre todas as linhas
        Palavras *Palavra_atual = atual->Primeira_palavra_da_linha;

        for(int j = 0; j<atual->num_de_palavras;j++){ // Percorre todas as palavras da linha "i"

            if(strcmp(palavra,Palavra_atual->palavra)==0){
                ocorrencias->linhas = realloc(ocorrencias->linhas,sizeof(int)*(aux+1));
                ocorrencias->colunas = realloc(ocorrencias->colunas, sizeof(int)*(aux)+1);
                ocorrencias->colunas[aux] = Palavra_atual->coluna;
                ocorrencias->linhas[aux] = Palavra_atual->linha;
                ocorrencias->qtd_de_ocorrencias++;
                aux++;

            }

            Palavra_atual = Palavra_atual->proxima;
        }
        atual = atual->linhaBaixo;
    }

    return ocorrencias;

}

void mostra_ocorrencias_Da_palavra(char *palavra, Multilista *multilista){

    Ocorrencias *ocorrencias = Ponteiro_Ocorrencias_da_palavra(palavra,multilista);
    if(ocorrencias->qtd_de_ocorrencias == 0){
        return;
    }

    printf("A palavra %s aparece em: ",palavra);
    for(int i = 0; i<ocorrencias->qtd_de_ocorrencias;i++){
        printf("%d.%d | ",ocorrencias->linhas[i],ocorrencias->colunas[i]);
    }

}

int Qtd_Total_de_palavras(Multilista *multilista){

    int qtd = 0;
    Linhas *atual = multilista->Primeira_linha;
    for(int i = 0; i<multilista->numLinhas;i++){
        qtd+=atual->num_de_palavras;
        atual = atual->linhaBaixo;
    }
    return qtd;

}

//caso alguma palavra seja adicionada ou removida, os valores de coluna das demais palavras
//ficariam desorganizados, pois a quantidade de palavras ja nao eh mais a mesma,
//a funcao abaixo organiza as colunas apos alguma palavra ser removida.
void Organizar_Linhas_Colunas(Multilista *multilista, int linha, int coluna, int Tamanho, char *palavra){
    Linhas *atual = multilista->Primeira_linha;

    for(int i  = 0; i<multilista->numLinhas;i++){ //Percorre todas as linhas
        Palavras *Palavra_atual = atual->Primeira_palavra_da_linha;

        for(int j = 0; j<atual->num_de_palavras;j++){ // Percorre todas as palavras da linha "i"

            if(Palavra_atual->linha == linha && Palavra_atual->coluna >= coluna && strcmp(Palavra_atual->palavra,palavra) != 0 ){
                if(Tamanho>=0){
                    Palavra_atual->coluna -= Tamanho+1;
                }else{
                    int tamanho = -Tamanho;
                    Palavra_atual->coluna += tamanho+1;
                }
            }
            Palavra_atual = Palavra_atual->proxima;
        }
        atual = atual->linhaBaixo;
    }
}

void removerLinha(int linha, Multilista *multilista){

    Linhas *atual = multilista->Primeira_linha;

    if(linha==1){ // Caso a linha a ser removida seja a primeira
        multilista->Primeira_linha = atual->linhaBaixo;
        multilista->numLinhas--;
        free(atual);
        return;
    }


    for(int i = 1; i<multilista->numLinhas+1;i++){

        if(i == multilista->numLinhas){ // caso a linha a ser removida seja ultima
            free(atual);
            multilista->numLinhas--;
            return;
        }

        else if(linha == i){ // remove a linha "i"
            atual->linhaCima->linhaBaixo = atual->linhaBaixo;
            atual->linhaBaixo->linhaCima = atual->linhaCima;
            free(atual); // remove a linha
            multilista->numLinhas--; // numero de linhas é reduzido em 1
            return;
        }

        atual = atual->linhaBaixo;
    }

}

int Remover_Palavra_Linha_Coluna(Multilista *multilista, int linha, int coluna){
    Linhas *atual = multilista->Primeira_linha;

    for(int i  = 0; i<multilista->numLinhas;i++){

        Palavras *Palavra_atual = atual->Primeira_palavra_da_linha;

        for(int j = 1; j<atual->num_de_palavras+1;j++){

            if(Palavra_atual->linha == linha && Palavra_atual->coluna == coluna){

                //para remover a ultima palavra do texto, obs: nao precisa organizar nesse caso
                if(j==atual->num_de_palavras){

                    //essa situacao eh para caso só tenha 1 palavra na linha.
                    if(atual->num_de_palavras==1){
                        free(Palavra_atual); // remove a palavra
                        removerLinha(linha,multilista);
                        return 1;

                        }

                    //caso tenha mais de 1 palavra na linha, e a palavra a ser removida eh a ultima da linha
                    else{
                        Palavra_atual->anterior->proxima = NULL;
                        free(Palavra_atual);
                        atual->num_de_palavras--;
                        return 1;
                    }


                }

                //para remover a primeira palavra do texto
                else if(j == 1 ){
                    Organizar_Linhas_Colunas(multilista,linha,coluna,strlen(Palavra_atual->palavra),Palavra_atual->palavra);
                    atual->Primeira_palavra_da_linha = Palavra_atual->proxima;
                    free(Palavra_atual);
                    atual->num_de_palavras--;
                    return 1;
                }

                Organizar_Linhas_Colunas(multilista,linha,coluna,strlen(Palavra_atual->palavra),Palavra_atual->palavra);
                Palavra_atual->anterior->proxima = Palavra_atual->proxima;
                Palavra_atual->proxima->anterior = Palavra_atual->anterior;
                free(Palavra_atual);
                atual->num_de_palavras--;
                return 1;
            }
            Palavra_atual = Palavra_atual->proxima;
        }
        atual = atual->linhaBaixo;
    }
    return 0;

}


int Remover_Palavra(Multilista *multilista, char *palavra ){
    Ocorrencias *ocorrencias = Ponteiro_Ocorrencias_da_palavra(palavra,multilista);
    int a = 0;
    for(int i = 0; i<ocorrencias->qtd_de_ocorrencias;i++){
       if(Remover_Palavra_Linha_Coluna(multilista,ocorrencias->linhas[i],ocorrencias->colunas[i])==1){
        a +=1;
       }else{
        return 0;
       }
    }
    if(a == ocorrencias->qtd_de_ocorrencias){
        return 1;
    }
}

int Editar_Palavra_Linha_Coluna(Multilista *multilista, int linha, int coluna,char *palavra){
    Linhas *atual = multilista->Primeira_linha;

    for(int i  = 0; i<multilista->numLinhas;i++){
        Palavras *Palavra_atual = atual->Primeira_palavra_da_linha;
        for(int j = 0; j<atual->num_de_palavras;j++){
            if(Palavra_atual->linha == linha && Palavra_atual->coluna == coluna){
                int tamanho = strlen(Palavra_atual->palavra) - (strlen(palavra));
                strcpy(Palavra_atual->palavra,palavra);
                Organizar_Linhas_Colunas(multilista,linha,coluna,tamanho,palavra);
                return 1;
            }
            Palavra_atual = Palavra_atual->proxima;
        }
        atual = atual->linhaBaixo;
    }
    return 0;
}

int Adicionar_Palavra(Multilista *multilista, int linha, int coluna,char *palavra){
        Linhas *atual = multilista->Primeira_linha;

    for(int i  = 0; i<multilista->numLinhas;i++){
        Palavras *Palavra_atual = atual->Primeira_palavra_da_linha;
        for(int j = 0; j<atual->num_de_palavras;j++){
            if(Palavra_atual->linha == linha && Palavra_atual->coluna == coluna){
                Palavras *Palavra_nova = malloc(sizeof(Palavras));
                strcpy(Palavra_nova->palavra,palavra);
                Palavra_nova->linha = linha;
                Palavra_nova->coluna = coluna;
                Palavra_atual->anterior->proxima = Palavra_nova;
                Palavra_nova->anterior = Palavra_atual->anterior;
                Palavra_atual->anterior = Palavra_nova;
                Palavra_nova->proxima = Palavra_atual;
                atual->num_de_palavras += 1;
                Organizar_Linhas_Colunas(multilista,linha,coluna,-(strlen(palavra)),palavra);
                return 1;
            }
            Palavra_atual = Palavra_atual->proxima;
        }
        atual = atual->linhaBaixo;
    }
    return 0;
}

int Adicionar_Palavra_Comeco(Multilista *multilista,char *palavra,int linha){
    Linhas *atual = multilista->Primeira_linha;

    for(int i  = 0; i<multilista->numLinhas;i++){
        Palavras *Palavra_atual = atual->Primeira_palavra_da_linha;
        if(Palavra_atual->linha == linha){
            Palavras *Palavra_nova = malloc(sizeof(Palavras));
            strcpy(Palavra_nova->palavra,palavra);
            Palavra_nova->linha = linha;
            Palavra_nova->coluna = Palavra_atual->coluna;
            Palavra_nova->anterior = NULL;
            Palavra_nova->proxima = Palavra_atual;
            Palavra_atual->anterior = Palavra_nova;
            atual->num_de_palavras += 1;
            atual->Primeira_palavra_da_linha = Palavra_nova;
            Organizar_Linhas_Colunas(multilista,linha,1,-(strlen(palavra)),palavra);
            return 1;
        }
        atual = atual->linhaBaixo;
    }
    return 0;
}

int Adicionar_Palavra_Fim(Multilista *multilista,char *palavra,int linha){
    Linhas *atual = multilista->Primeira_linha;

    for(int i  = 1; i<multilista->numLinhas+1;i++){
    Palavras *Palavra_atual = atual->Primeira_palavra_da_linha;
        for(int j = 1; j<atual->num_de_palavras+1;j++){
                if( i == linha && j == atual->num_de_palavras){
                Palavras *Palavra_nova = malloc(sizeof(Palavras));
                strcpy(Palavra_nova->palavra,palavra);
                Palavra_nova->linha = linha;
                Palavra_nova->coluna = Palavra_atual->coluna+strlen(Palavra_atual->palavra)+1;
                Palavra_atual->proxima = Palavra_nova;
                Palavra_nova->anterior = Palavra_atual;
                Palavra_nova->proxima = NULL;
                atual->num_de_palavras += 1;
                return 1;
            }
            Palavra_atual = Palavra_atual->proxima;
        }
    atual = atual->linhaBaixo;
    }
    return 0;
}

Ocorrencias* Ponteiro_Ocorrencias_SubString(char *palavra, Multilista *multilista){

    Ocorrencias *ocorrencias = malloc(sizeof(Ocorrencias));
    ocorrencias->qtd_de_ocorrencias = 0;
    ocorrencias->colunas = malloc(0);
    ocorrencias->linhas = malloc(0);
    strcpy(ocorrencias->palavra,palavra);

    Linhas *atual = multilista->Primeira_linha;

    int aux = 0; // vai ajudar a alocar na posiçao correta dos vetores, e tambem para realocar os vetores

    for(int i  = 0; i<multilista->numLinhas;i++){
        Palavras *Palavra_atual = atual->Primeira_palavra_da_linha;

        for(int j = 0; j<atual->num_de_palavras;j++){

            if(strstr(Palavra_atual->palavra,palavra)!=NULL){
                ocorrencias->linhas = realloc(ocorrencias->linhas,sizeof(int)*(aux+1));
                ocorrencias->colunas = realloc(ocorrencias->colunas, sizeof(int)*(aux)+1);
                ocorrencias->colunas[aux] = Palavra_atual->coluna;
                ocorrencias->linhas[aux] = Palavra_atual->linha;
                ocorrencias->qtd_de_ocorrencias++;
                aux++;

            }

            Palavra_atual = Palavra_atual->proxima;
        }
        atual = atual->linhaBaixo;
    }

    return ocorrencias;

}

void mostra_ocorrencias_Substring(char *Substring, Multilista *multilista){

    Ocorrencias *ocorrencias = Ponteiro_Ocorrencias_SubString(Substring,multilista);
    if(ocorrencias->qtd_de_ocorrencias == 0){
        printf("A Substring %s aparece 0 vezes.\n",Substring);
        return;
    }

    printf("%d resultados para a substring %s: ",ocorrencias->qtd_de_ocorrencias,Substring);
    for(int i = 0; i<ocorrencias->qtd_de_ocorrencias;i++){
        printf("%d.%d | ",ocorrencias->linhas[i],ocorrencias->colunas[i]);
    }
    printf("\n");

}


int escolhaLugar_Nova_Palavra(Multilista *multilista){
    int escolha = -1;
    char s[100];
    int linha,col;
    printf("1-comeco \n2-meio(lugar especifico) \n3-fim\n");
    scanf("%d",&escolha);
    switch(escolha){
        case 1:
            printf("Digite a linha\n");
            scanf("%d",&linha);
            printf("Digite uma palavra\n");
            scanf("%s",s);
            if(Adicionar_Palavra_Comeco(multilista,s,linha)==1){
                printf("Palavra adicionada!");
            }else{
                printf("Ocorreu um erro ao adicionar a palavra.");
            }
            return 1;
            break;
        case 3:
            printf("Digite a linha\n");
            scanf("%d",&linha);
            printf("Digite uma palavra\n");
            scanf("%s",s);
            if(Adicionar_Palavra_Fim(multilista,s,linha)==1){
                printf("Palavra adicionada!");
            }else{
                printf("Ocorreu um erro ao adicionar a palavra.");
            }
            return 1;
            break;
        case 2:
            printf("Digite a linha\n");
            scanf("%d",&linha);
            printf("digite a coluna\n");
            scanf("%d",&col);
            printf("Digite uma palavra\n");
            scanf("%s",s);
            if(Adicionar_Palavra(multilista,linha,col,s)==1){
                printf("Palavra adicionada!");
            }else{
                printf("Ocorreu um erro ao adicionar a palavra.");
            }
            return 1;
            break;
    }
}

void mostrar_Todas_AS_Linhas_File(Multilista *multilista) {
    FILE *file = fopen(NOME_ARQUIVO, "w");
    Linhas *atual = multilista->Primeira_linha;
    for (int i = 0; i < multilista->numLinhas; i++) {
        Palavras *Palavra_Atual = atual->Primeira_palavra_da_linha;
        while (Palavra_Atual != NULL) {
            fprintf(file, "%s ", Palavra_Atual->palavra);
            Palavra_Atual = Palavra_Atual->proxima;
        }
        fprintf(file,"\n");
        atual = atual->linhaBaixo;
    }
    fclose(file);
}

void Atualizar_Arquivo(Multilista *multilista) {

    const char* nomeAtual = NOME_ARQUIVO;

    char novoNome[100]; // Tamanho adequado para o novo nome
    sprintf(novoNome, "OLD_%s", NOME_ARQUIVO);

    int status = rename(nomeAtual, novoNome);
    if (status != 0) {
        printf("Erro ao renomear o arquivo %s.\n",NOME_ARQUIVO);
        return;
    }
    FILE *arquivo = fopen(NOME_ARQUIVO, "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n",NOME_ARQUIVO);
        return;
    }

    mostrar_Todas_AS_Linhas_File(multilista);

    fclose(arquivo);
}
void menu(){
    printf("\n1- Mostrar Texto\n");
    printf("2- Buscar palavra\n");
    printf("3- Remover uma palavra\n");
    printf("4- Remover palavra em linha e coluna especificas\n");
    printf("5- Exibir Numero de occorencias de uma palavra\n");
    printf("6- Total de palavras no texto\n");
    printf("7- Editar uma palavra\n");
    printf("8- Inserir nova palavra\n");
    printf("9- pesquisar sub-string\n");
    printf("10- Atualizar arquivo\n");
    printf("11- sair\n");

}

 int main(){
    Multilista *multilista = malloc(sizeof(multilista));
    Alocar_texto(multilista);

    int escolha = -1;
    while(escolha != 11){
        menu();
        scanf("%d",&escolha);
        switch(escolha){
            case 1:
                mostrar_Todas_AS_Linhas(multilista);
                break;
            case 2:
                printf("Digite uma palavra\n");
                char s[100];
                scanf("%s",s);
                mostra_ocorrencias_Da_palavra(s,multilista);
                break;
            case 3:
                printf("Digite uma palavra\n");
                scanf("%s",s);
                if(Remover_Palavra(multilista,s) == 1){
                    printf("Todas as ocorrencias da palavra, foram removidas!");
                }else{
                    printf("Ocorreu um erro ao remover a palavra.");
                }
                break;
            case 4:
                printf("Digite a linha\n");
                int linha,col;
                scanf("%d",&linha);
                printf("digite a coluna\n");
                scanf("%d",&col);
                if(Remover_Palavra_Linha_Coluna(multilista,linha,col) == 1){
                    printf("Palavra removida!");
                }else{
                    printf("Ocorreu um erro ao remover a palavra.");
                }
                break;
            case 5:
                printf("Digite uma palavra\n");
                scanf("%s",s);
                Ocorrencias *ocorrencias = Ponteiro_Ocorrencias_da_palavra(s,multilista);
                printf("A palavra %s aparece %d vezes\n",s,ocorrencias->qtd_de_ocorrencias);
                mostra_ocorrencias_Da_palavra(s,multilista);
                break;
            case 6:
                printf("A quantidade total de palavra eh %d\n",Qtd_Total_de_palavras(multilista));
                break;
            case 7:
                printf("Digite a linha\n");
                scanf("%d",&linha);
                printf("digite a coluna\n");
                scanf("%d",&col);
                printf("Digite uma palavra\n");
                scanf("%s",s);
                if(Editar_Palavra_Linha_Coluna(multilista,linha,col,s)==1){
                    printf("\nPalavra alterada!");
                }else{
                    printf("Ocorreu um erro ao editar a palavra.");
                }
                break;
            case 8:
                escolhaLugar_Nova_Palavra(multilista);
                break;
            case 9:
                printf("Digite a subString:");
                char sub[100];
                scanf("%s",sub);
                mostra_ocorrencias_Substring(sub,multilista);
                break;
            case 10:
                Atualizar_Arquivo(multilista);
                break;
            case 11:
                return 0;

        }
    }



    return 0;
 }
