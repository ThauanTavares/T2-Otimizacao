#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#include <stdbool.h>

#define MAX_GROUPS 100

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

bool covers_all_groups(Candidate* selected, int total_groups, bool *covered) {
    Candidate* temp = selected;

    while (temp != NULL) {
        for (int i = 0; i < temp->group_set.group_count; i++) {
            covered[temp->group_set.groups[i] - 1] = true;
        }
        temp = temp->next;
    }

    for (int i = 0; i < total_groups; i++) {
        if (!covered[i]) return false;
    }
    return true;
}

int bdada(int l, Candidate* e, Candidate* f) {
    int *gruposCobertos = (int *)calloc(l + 1, sizeof(int));
    int gruposCobrindo = 0;
    int E_size = 0;

    Candidate* current = e;
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

int bdefault(int l, Candidate* e, Candidate* f) {
    bool covered[MAX_GROUPS] = {false};
    int uncovered_groups = 0;

    // Verifica quais grupos já estão cobertos pelos candidatos em E
    if (covers_all_groups(e, l, covered))
        return 0;

    // Conta o número de grupos não cobertos
    for (int i = 0; i < l; i++) {
        if (!covered[i]) {
            uncovered_groups++;
        }
    }

    // Calcula a máxima cobertura possível entre os candidatos restantes
    int max_cover = 0;
    Candidate* temp = f;
    while (temp != NULL) {
        if (temp->group_set.group_count > max_cover) {
            max_cover = temp->group_set.group_count;
        }
        temp = temp->next;
    }

    // Calcula o limite inferior de candidatos adicionais necessários
    int additional_candidates = (uncovered_groups + max_cover - 1) / max_cover; // Ceiling division

    return size_candidate(&e) + additional_candidates;
}


bool feasible(int l, Candidate* e, Candidate* f){
    bool covered[MAX_GROUPS];
    for (int i = 0; i < l; i++) {
        covered[i] = false;
    }
    
    covers_all_groups(e, l, covered);
    return covers_all_groups(f, l, covered);
}

bool optimal(int e_size, int melhorE){
    if(e_size >= melhorE)
        return false;
    return true;
}

void branchAndBound(int l, Candidate* currentE, Candidate* currentF, int e_size, Candidate** melhorE, int* melhorESize, int (*boundFunction)(int, Candidate*, Candidate*), bool is_feasible, bool is_optimal) { 
    if (is_feasible && !feasible(l, currentE, currentF)){
        return;
    }

    if (is_optimal && !optimal(e_size, *melhorESize)){
        return;
    }
    
    if (boundFunction(l, currentE, currentF) >= *melhorESize)
        return;
    
    bool covered[MAX_GROUPS];
    for (int i = 0; i < l; i++)
        covered[i] = false;

    if (covers_all_groups(currentE, l, covered)) {
        if (e_size < *melhorESize) {
            *melhorESize = e_size;

            // Libera a lista atual de melhorE
            Candidate* temp = *melhorE;
            while (temp != NULL) {
                Candidate* next = temp->next;
                remove_candidate(&temp, temp);
                temp = next;
            }

            // Atualiza melhorE para o conjunto atual de candidatos
            *melhorE = NULL;
            Candidate* iter = currentE;
            while (iter != NULL) {
                add_candidate(melhorE, iter->id, iter->group_set.groups, iter->group_set.group_count);
                iter = iter->next;
            }
        }
        return;
    }

    if (currentF == NULL)
        return;

    
    // Adiciona o candidato atual ao conjunto E
    Candidate * aux = add_candidate(&currentE, currentF->id, currentF->group_set.groups, currentF->group_set.group_count);

    // Remove o candidato da lista F
    currentF = *remove_candidate(&currentF, currentF);

    // Explora o ramo incluindo o candidato atual
    branchAndBound(l, currentE, currentF, e_size + 1, melhorE, melhorESize, boundFunction, is_feasible, is_optimal);

    // Remove o candidato da lista F
    currentE = *remove_candidate(&currentE, aux);

    // Explora o ramo excluindo o candidato atual
    branchAndBound(l, currentE, currentF, e_size, melhorE, melhorESize, boundFunction, is_feasible, is_optimal);

    free(aux);
}

int main(int argc, char *argv[]) {
    bool funcaoLimitantePadrao = true;
    bool cortesViabilidade = true;
    bool cortesOtimalidade = true;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            funcaoLimitantePadrao = false;
        } else if (strcmp(argv[i], "-f") == 0) {
            cortesViabilidade = false;
        } else if (strcmp(argv[i], "-o") == 0) {
            cortesOtimalidade = false;
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
    printf("l: %d n: %d\n", l, n);
    print_candidates(head);

    Candidate* melhorE = NULL;
    int melhorESize = n + 1;

    int (*boundFunction)(int, Candidate*, Candidate*) = bdefault;

    if (funcaoLimitantePadrao) {
        printf("Usando a função limitante default.\n");
    } else {
        printf("Usando a função limitante fornecida pelo professor.\n");
        boundFunction = bdada;
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

    Candidate* f = NULL;
    Candidate* a = head;
    while (a != NULL) {
        add_candidate(&f, a->id, a->group_set.groups, a->group_set.group_count);
        a = a->next;
    }

    branchAndBound(l, NULL, f, 0, &melhorE, &melhorESize, boundFunction, cortesViabilidade, cortesOtimalidade);

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
