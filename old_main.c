#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

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

int Bdada(int l, Candidate* head) {
    int *gruposCobertos = (int *)calloc(l + 1, sizeof(int));
    int gruposCobrindo = 0;
    int E_size = 0;

    Candidate* current = head;
    while (current != NULL) {
        E_size++;
        for (int j = 0; j < current->group_set.group_count; j++) {
            int grupo = current->group_set.groups[j];
            if (!gruposCobertos[grupo]) {
                gruposCobertos[grupo] = 1;
                gruposCobrindo++;
            }
        }
        current = current->next;
    }

    free(gruposCobertos);

    if (gruposCobrindo == l) {
        return E_size;
    } else {
        return E_size + 1;
    }
}



void branchAndBound(int l, Candidate* head, Candidate* tail, Candidate* currentE, int E_size, Candidate** melhorE, int* melhorESize, int (*boundFunction)(int, Candidate*)) {
    if (boundFunction(l, currentE) >= *melhorESize) {
        return;
    }

    int *gruposCobertos = (int *)calloc(l + 1, sizeof(int));
    int gruposCobrindo = 0;

    Candidate* temp = currentE;
    while (temp != NULL) {
        for (int j = 0; j < temp->group_set.group_count; j++) {
            int grupo = temp->group_set.groups[j];
            if (!gruposCobertos[grupo]) {
                gruposCobertos[grupo] = 1;
                gruposCobrindo++;
            }
        }
        temp = temp->next;
    }

    if (gruposCobrindo == l) {
        if (E_size < *melhorESize) {
            *melhorESize = E_size;
            // Atualize melhorE para o conjunto atual de candidatos
            Candidate* iter = currentE;
            *melhorE = NULL;
            while (iter != NULL) {
                add_candidate(melhorE, iter->id, iter->group_set.groups, iter->group_set.group_count);
                iter = iter->next;
            }
        }
        free(gruposCobertos);
        return;
    }

    free(gruposCobertos);

    if (tail == NULL) {
        return;
    }

    Candidate* novoE = create_candidate(tail->id, tail->group_set.groups, tail->group_set.group_count);
    novoE->next = currentE;
    if (currentE != NULL) {
        currentE->prev = novoE;
    }

    Candidate* novoTail = tail->prev;
    if (novoTail != NULL) {
        novoTail->next = NULL;
    }

    branchAndBound(l, head, novoTail, novoE, E_size + 1, melhorE, melhorESize, boundFunction);
    branchAndBound(l, head, novoTail, currentE, E_size, melhorE, melhorESize, boundFunction);

    free(novoE->group_set.groups);
    free(novoE);
}




int main(int argc, char *argv[]) {
    int funcaoLimitantePadrao = 1;
    int cortesViabilidade = 1;
    int cortesOtimalidade = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            funcaoLimitantePadrao = 0;
        } else if (strcmp(argv[i], "-f") == 0) {
            cortesViabilidade = 0;
        } else if (strcmp(argv[i], "-o") == 0) {
            cortesOtimalidade = 0;
        } else {
            printf("Uso: %s [-a] [-f] [-o]\n", argv[0]);
            printf("    -a    Usa a função limitante fornecida pelo professor\n");
            printf("    -f    Desativa os cortes por viabilidade\n");
            printf("    -o    Desativa os cortes por otimalidade\n");
            printf("    (Sem opções) Usa a função limitante default com todos os cortes ativos\n");
            return 1;
        }
    }

    int l, n;
    if (scanf("%d %d", &l, &n) != 2) {
        fprintf(stderr, "Erro na leitura dos valores de ` e `n`\n");
        return 1;
    }

    Candidate* head = NULL;
    for (int i = 0; i < n; i++) {
        int s;
        if (scanf("%d", &s) != 1) {
            fprintf(stderr, "Erro na leitura do número de grupos do candidato %d\n", i + 1);
            return 1;
        }
        int* groups = (int*)malloc(s * sizeof(int));
        for (int j = 0; j < s; j++) {
            if (scanf("%d", &groups[j]) != 1) {
                fprintf(stderr, "Erro na leitura dos grupos do candidato %d\n", i + 1);
                return 1;
            }
        }
        add_candidate(&head, i+1, groups, s);
        free(groups);
    }

    print_candidates(head);

    Candidate* melhorE = NULL;
    int melhorESize = n + 1;

    int (*boundFunction)(int, Candidate*) = Bdada;

    if (funcaoLimitantePadrao) {
        printf("Usando a função limitante default.\n");
        // Aqui você pode definir outra função limitante, se necessário
    } else {
        printf("Usando a função limitante fornecida pelo professor.\n");
    }

    if (cortesViabilidade) {
        printf("Cortes por viabilidade estão ativos.\n");
    } else {
        printf("Cortes por viabilidade estão desativados.\n");
    }

    if (cortesOtimalidade) {
        printf("Cortes por otimalidade estão ativos.\n");
    } else {
        printf("Cortes por otimalidade estão desativados.\n");
    }

    Candidate* tail = head;
    while (tail->next != NULL) {
        tail = tail->next;
    }

    branchAndBound(l, head, tail, NULL, 0, &melhorE, &melhorESize, boundFunction);

    if (melhorESize <= n) {
        printf("Número mínimo de candidatos: %d\n", melhorESize);
        printf("Candidatos escolhidos: ");
        Candidate* temp = melhorE;
        while (temp != NULL) {
            printf("%d ", temp->id);
            temp = temp->next;
        }
        printf("\n");
    } else {
        printf("Não foi possível encontrar uma solução.\n");
    }

    // Libera a memória alocada
    Candidate* temp = head;
    while (temp != NULL) {
        Candidate* next = temp->next;
        free(temp->group_set.groups);
        free(temp);
        temp = next;
    }

    temp = melhorE;
    while (temp != NULL) {
        Candidate* next = temp->next;
        free(temp->group_set.groups);
        free(temp);
        temp = next;
    }

    return 0;
}
