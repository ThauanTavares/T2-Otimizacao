#include "lib.h"
#include <stdbool.h>

#define MAX_GROUPS 100

int calculate_incremental_coverage(Candidate* candidate, bool* covered) {
    int incremental_coverage = 0;
    for (int i = 0; i < candidate->group_set.group_count; i++) {
        if (!covered[candidate->group_set.groups[i]]) {
            incremental_coverage++;
        }
    }
    return incremental_coverage;
}

bool covers_all_groups(Candidate* selected, int total_groups, bool *covered) {
    for (int i = 0; i < total_groups; i++) {
        covered[i] = false;
    }
    Candidate* temp = selected;

    while (temp != NULL) {
        printf("aaaaadddaaa\n");
        for (int i = 0; i < temp->group_set.group_count; i++) {
            covered[temp->group_set.groups[i]] = true;
        }
        temp = temp->next;
    }

    for (int i = 0; i < total_groups; i++) {
        if (!covered[i]) return false;
    }
    return true;
}

void branch_and_bound_recursive(Candidate* remaining, Candidate* selected, int total_groups, int* best_solution_size, Candidate** best_solution) {
    bool *covered = (bool *)calloc(total_groups, sizeof(bool));
    bool is_covered = covers_all_groups(selected, total_groups, covered);
    
    if (remaining == NULL) {
        if (is_covered && (*best_solution == NULL || *best_solution_size > 1)) {
            *best_solution_size = 1;
            *best_solution = selected;
        }
        return;
    }
    
    if (selected != NULL && !is_covered) {
        return;
    }
        
    int bound = *best_solution_size;
    if (bound > *best_solution_size) {
        return;
    }

    Candidate* best_candidate = NULL;
    int max_incremental_coverage = -1;


    Candidate* temp = selected;
    temp = remaining;
    while (temp != NULL) {
        int incremental_coverage = calculate_incremental_coverage(temp, covered);
        if (incremental_coverage > max_incremental_coverage) {
            max_incremental_coverage = incremental_coverage;
            best_candidate = temp;
        }
        temp = temp->next;
    }

    printf("aaaaabcbaaa\n");

    if (best_candidate != NULL) {
        printf("aaaaabcddbaaa\n");
        add_candidate(&remaining, best_candidate->id, best_candidate->group_set.groups, best_candidate->group_set.group_count);
        remove_candidate(&selected, best_candidate);

        if (covers_all_groups(selected, total_groups, covered) && (*best_solution == NULL || *best_solution_size > 1 + best_candidate->id)) {
            *best_solution_size = best_candidate->id + 1;
            *best_solution = selected;
        }

        branch_and_bound_recursive(remaining, selected, total_groups, best_solution_size, best_solution);

        add_candidate(&remaining, best_candidate->id, best_candidate->group_set.groups, best_candidate->group_set.group_count);
        remove_candidate(&selected, best_candidate);
    }
}

void branch_and_bound(Candidate* candidates, int total_groups, bool use_viability_cut, bool use_optimality_cut) {
    Candidate* selected = NULL;
    int best_solution_size = MAX_GROUPS;
    Candidate* best_solution = NULL;

    branch_and_bound_recursive(candidates, selected, total_groups, &best_solution_size, &best_solution);

    printf("Melhor solução:\n");
    print_candidates(best_solution);
}

int main() {
    Candidate* head = NULL;

    int groups1[] = {0, 1};
    int groups2[] = {1, 2};
    int groups3[] = {2, 3};
    int groups4[] = {3, 4};

    add_candidate(&head, 1, groups1, 2);
    add_candidate(&head, 2, groups2, 2);
    add_candidate(&head, 3, groups3, 2);
    add_candidate(&head, 4, groups4, 2);

    printf("Lista de candidatos:\n");
    print_candidates(head);

    int total_groups = 5;
    bool use_viability_cut = true;
    bool use_optimality_cut = true;

    branch_and_bound(head, total_groups, use_viability_cut, use_optimality_cut);

    while (head != NULL) {
        Candidate* temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
