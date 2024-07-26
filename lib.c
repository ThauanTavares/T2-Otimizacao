#include "lib.h"

Candidate* create_candidate(int id, int* groups, int group_count) {
    Candidate* new_candidate = (Candidate*)malloc(sizeof(Candidate));
    new_candidate->id = id;
    new_candidate->group_set.group_count = group_count;
    new_candidate->group_set.groups = (int*)malloc((group_count) * sizeof(int));
    for (int i = 0; i < group_count; i++) {
        new_candidate->group_set.groups[i] = groups[i];
    }
    new_candidate->next = NULL;
    new_candidate->prev = NULL;
    return new_candidate;
}

Candidate* add_candidate(Candidate** head, int id, int* groups, int group_count) {
    Candidate* new_candidate = create_candidate(id, groups, group_count);
    if (*head == NULL) {
        *head = new_candidate;
    } else {
        Candidate* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_candidate;
        new_candidate->prev = temp;
    }
    return new_candidate;
}

Candidate** remove_candidate(Candidate** head, Candidate* candidate) {
    if (*head == NULL || candidate == NULL) return NULL;

    if (*head == candidate) {
        *head = candidate->next;
    }

    if (candidate->next != NULL) {
        candidate->next->prev = candidate->prev;
    }

    if (candidate->prev != NULL) {
        candidate->prev->next = candidate->next;
    }

    return head;
}

int size_candidate(Candidate** head) {
    if (*head == NULL) 
        return 0;

    return size_candidate(&((*head)->next)) + 1;
}

void print_candidates(Candidate* head) {
    Candidate* temp = head;
    while (temp != NULL) {
        printf("Candidato ID: %d\n", temp->id);
        printf("Grupos: ");
        for (int i = 0; i < temp->group_set.group_count; i++) {
            printf("%d ", temp->group_set.groups[i]);
        }
        printf("\n");
        temp = temp->next;
    }
}
