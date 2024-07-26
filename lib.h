#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *groups;
    int group_count;
} GroupSet;

typedef struct Candidate {
    int id;
    GroupSet group_set;
    struct Candidate* next;
    struct Candidate* prev;
} Candidate;

Candidate* create_candidate(int id, int* groups, int group_count);

Candidate* add_candidate(Candidate** head, int id, int* groups, int group_count);

Candidate** remove_candidate(Candidate** head, Candidate* candidate);

int size_candidate(Candidate** head);

void print_candidates(Candidate* candidate);