#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void instrUso(char *program_name) {
    printf("Uso: %s [-a] [-f] [-o]\n", program_name);
    printf("    -a    Usa a função limitante fornecida pelo professor\n");
    printf("    -f    Desativa os cortes por viabilidade\n");
    printf("    -o    Desativa os cortes por otimalidade\n");
    printf("    (Sem opções) Usa a função limitante default com todos os cortes ativos\n");
}

void imprimirDados(int l, int n, int *numGrupos, int **candidatos) {
    printf("Número de grupos (`): %d\n", l);
    printf("Número de candidatos (n): %d\n", n);
    for (int i = 0; i < n; i++) {
        printf("Candidato %d pertence a %d grupos: ", i + 1, numGrupos[i]);
        for (int j = 0; j < numGrupos[i]; j++) {
            printf("%d ", candidatos[i][j]);
        }
        printf("\n");
    }
}

int Bdada(int l, int *E, int E_size, int **candidatos, int *numGrupos) {
    // Contar os grupos cobertos pelos candidatos em E
    int *gruposCobertos = (int *)calloc(l + 1, sizeof(int));
    int gruposCobrindo = 0;

    for (int i = 0; i < E_size; i++) {
        int candidato = E[i];
        for (int j = 0; j < numGrupos[candidato]; j++) {
            int grupo = candidatos[candidato][j];
            if (!gruposCobertos[grupo]) {
                gruposCobertos[grupo] = 1;
                gruposCobrindo++;
            }
        }
    }

    free(gruposCobertos);

    if (gruposCobrindo == l) {
        return E_size;
    } else {
        return E_size + 1;
    }
}


void branchAndBound(int l, int n, int **candidatos, int *numGrupos, int *E, int E_size, int *melhorE, int *melhorESize, int *F, int F_size, int (*boundFunction)(int, int *, int, int **, int *)) {
    if (boundFunction(l, E, E_size, candidatos, numGrupos) >= *melhorESize) {
        return;
    }

    int *gruposCobertos = (int *)calloc(l + 1, sizeof(int));
    int gruposCobrindo = 0;
    for (int i = 0; i < E_size; i++) {
        int candidato = E[i];
        for (int j = 0; j < numGrupos[candidato]; j++) {
            int grupo = candidatos[candidato][j];
            if (!gruposCobertos[grupo]) {
                gruposCobertos[grupo] = 1;
                gruposCobrindo++;
            }
        }
    }

    if (gruposCobrindo == l) {
        if (E_size < *melhorESize) {
            *melhorESize = E_size;
            memcpy(melhorE, E, E_size * sizeof(int));
        }
        free(gruposCobertos);
        return;
    }

    free(gruposCobertos);

    if (F_size == 0) {
        return;
    }

    int novoCandidato = F[F_size - 1];
    int *novoE = (int *)malloc((E_size + 1) * sizeof(int));
    memcpy(novoE, E, E_size * sizeof(int));
    novoE[E_size] = novoCandidato;

    int *novoF = (int *)malloc(F_size * sizeof(int));
    memcpy(novoF, F, F_size * sizeof(int));
    novoF[F_size - 1] = novoF[0];
    novoF[0] = novoF[F_size - 1];

    branchAndBound(l, n, candidatos, numGrupos, novoE, E_size + 1, melhorE, melhorESize, novoF, F_size - 1, boundFunction);
    branchAndBound(l, n, candidatos, numGrupos, E, E_size, melhorE, melhorESize, F, F_size - 1, boundFunction);

    free(novoE);
    free(novoF);
}



int main(int argc, char *argv[]) {
    int funcaoLimitantePadrao = 1; // Usar a função limitante default por padrão
    int cortesViabilidade = 1;  // Cortes por viabilidade ativos por padrão
    int cortesOtimalidade = 1;   // Cortes por otimalidade ativos por padrão

    // Processa os argumentos da linha de comando
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            funcaoLimitantePadrao = 0; // Usa a função limitante fornecida pelo professor
        } else if (strcmp(argv[i], "-f") == 0) {
            cortesViabilidade = 0;  // Desativa os cortes por viabilidade
        } else if (strcmp(argv[i], "-o") == 0) {
            cortesOtimalidade = 0;   // Desativa os cortes por otimalidade
        } else {
            instrUso(argv[0]);
            return 1;
        }
    }

    // Leitura dos valores de l e n
    int l, n;
    if (scanf("%d %d", &l, &n) != 2) {
        fprintf(stderr, "Erro na leitura dos valores de ` e `n`\n");
        return 1;
    }

    // Alocação dinâmica para armazenar os candidatos e os grupos que eles representam
    int **candidatos = (int **)malloc(n * sizeof(int *));
    int *numGrupos = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        int s;
        if (scanf("%d", &s) != 1) {
            fprintf(stderr, "Erro na leitura do número de grupos do candidato %d\n", i + 1);
            return 1;
        }
        numGrupos[i] = s;
        candidatos[i] = (int *)malloc(s * sizeof(int));
        for (int j = 0; j < s; j++) {
            if (scanf("%d", &candidatos[i][j]) != 1) {
                fprintf(stderr, "Erro na leitura dos grupos do candidato %d\n", i + 1);
                return 1;
            }
        }
    }
    imprimirDados(l,n,numGrupos,candidatos);

    int *E = (int *)malloc(n * sizeof(int));
    int E_size = 0;
    int *melhorE = (int *)malloc(n * sizeof(int));
    int melhorESize = n + 1;

    int *F = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        F[i] = i;
    }

    int (*boundFunction)(int, int *, int, int **, int *) = Bdada;







    // Exemplo de como a escolha das opções pode ser usada na implementação
    if (funcaoLimitantePadrao) {
        printf("Usando a função limitante default.\n");
        // Chame a função limitante default aqui
    } else {
        printf("Usando a função limitante fornecida pelo professor.\n");
        // Chame a função limitante fornecida pelo professor aqui
    }

    if (cortesViabilidade) {
        printf("Cortes por viabilidade estão ativos.\n");
        // Implementação dos cortes por viabilidade
    } else {
        printf("Cortes por viabilidade estão desativados.\n");
        // Implementação sem cortes por viabilidade
    }

    if (cortesOtimalidade) {
        printf("Cortes por otimalidade estão ativos.\n");
        // Implementação dos cortes por otimalidade
    } else {
        printf("Cortes por otimalidade estão desativados.\n");
        // Implementação sem cortes por otimalidade
    }


    branchAndBound(l, n, candidatos, numGrupos, E, E_size, melhorE, &melhorESize, F, n, boundFunction);

    if (melhorESize <= n) {
        printf("Número mínimo de candidatos: %d\n", melhorESize);
        printf("Candidatos escolhidos: ");
        for (int i = 0; i < melhorESize; i++) {
            printf("%d ", melhorE[i] + 1);
        }
        printf("\n");
    } else {
        printf("Não foi possível encontrar uma solução.\n");
    }

    for (int i = 0; i < n; i++) {
        free(candidatos[i]);
    }
    free(candidatos);
    free(numGrupos);
    free(E);
    free(melhorE);
    free(F);


    return 0;
}