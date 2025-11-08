#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PROD 50
#define MAX_SYMBOLS 20
#define MAX_ITEMS 100
#define MAX_STATES 50

typedef struct {
    char lhs[5];  
    char rhs[MAX_SYMBOLS];
} Production;

typedef struct {
    int prod_num;
    int dot_pos;
} Item;

typedef struct {
    Item items[MAX_ITEMS];
    int count;
    int state_num;
} ItemSet;

typedef struct {
    int from_state;
    int to_state;
    char symbol;
} Transition;

Production prods[MAX_PROD];
int prod_count = 0;
ItemSet states[MAX_STATES];
int state_count = 0;
Transition transitions[MAX_STATES * MAX_SYMBOLS];
int trans_count = 0;

char first[MAX_SYMBOLS][MAX_SYMBOLS];
int first_count[MAX_SYMBOLS];
char follow[MAX_SYMBOLS][MAX_SYMBOLS];
int follow_count[MAX_SYMBOLS];

#define ACTION_SHIFT 1
#define ACTION_REDUCE 2
#define ACTION_ACCEPT 3
#define ACTION_ERROR 0

typedef struct {
    int action_type;
    int value;       
} ActionEntry;

typedef struct {
    ActionEntry action[256];  
    int goto_table[26];    
} ParseTableRow;

ParseTableRow parse_table[MAX_STATES];
char terminals[MAX_SYMBOLS];
int terminal_count = 0;
char non_terminals[MAX_SYMBOLS];
int non_terminal_count = 0;

void trim(char *str) {
    int i, j = 0;
    for (i = 0; str[i]; i++) {
        if (!isspace(str[i])) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

int is_non_terminal(char c) {
    return isupper(c);
}

int is_non_terminal_str(const char *str) {
    if (strcmp(str, "S'") == 0) return 1;
    return (strlen(str) == 1 && isupper(str[0]));
}

int item_exists(ItemSet *set, Item item) {
    for (int i = 0; i < set->count; i++) {
        if (set->items[i].prod_num == item.prod_num && 
            set->items[i].dot_pos == item.dot_pos) {
            return 1;
        }
    }
    return 0;
}

void add_item(ItemSet *set, Item item) {
    if (!item_exists(set, item)) {
        set->items[set->count++] = item;
    }
}

char get_symbol_after_dot(int prod_num, int dot_pos) {
    if (dot_pos >= strlen(prods[prod_num].rhs)) {
        return '\0';
    }
    return prods[prod_num].rhs[dot_pos];
}

void closure(ItemSet *set) {
    int changed = 1;
    while (changed) {
        changed = 0;
        int current_count = set->count;
        
        for (int i = 0; i < current_count; i++) {
            Item item = set->items[i];
            char next_sym = get_symbol_after_dot(item.prod_num, item.dot_pos);
            
            if (next_sym != '\0' && is_non_terminal(next_sym)) {
                for (int j = 0; j < prod_count; j++) {
                    if (strlen(prods[j].lhs) == 1 && prods[j].lhs[0] == next_sym) {
                        Item new_item = {j, 0};
                        if (!item_exists(set, new_item)) {
                            add_item(set, new_item);
                            changed = 1;
                        }
                    }
                }
            }
        }
    }
}

void goto_set(ItemSet *from, ItemSet *to, char symbol) {
    to->count = 0;
    
    for (int i = 0; i < from->count; i++) {
        Item item = from->items[i];
        char next_sym = get_symbol_after_dot(item.prod_num, item.dot_pos);
        
        if (next_sym == symbol) {
            Item new_item = {item.prod_num, item.dot_pos + 1};
            add_item(to, new_item);
        }
    }
    
    if (to->count > 0) {
        closure(to);
    }
}

int item_sets_equal(ItemSet *s1, ItemSet *s2) {
    if (s1->count != s2->count) return 0;
    
    for (int i = 0; i < s1->count; i++) {
        int found = 0;
        for (int j = 0; j < s2->count; j++) {
            if (s1->items[i].prod_num == s2->items[j].prod_num &&
                s1->items[i].dot_pos == s2->items[j].dot_pos) {
                found = 1;
                break;
            }
        }
        if (!found) return 0;
    }
    return 1;
}

int find_state(ItemSet *set) {
    for (int i = 0; i < state_count; i++) {
        if (item_sets_equal(&states[i], set)) {
            return i;
        }
    }
    return -1;
}

void get_all_symbols(char symbols[], int *sym_count) {
    *sym_count = 0;
    int used[256] = {0};
    
    for (int i = 0; i < prod_count; i++) {
        if (strcmp(prods[i].lhs, "S'") != 0) {
            char lhs_char = prods[i].lhs[0];
            if (!used[(int)lhs_char]) {
                symbols[(*sym_count)++] = lhs_char;
                used[(int)lhs_char] = 1;
            }
        }
        
        for (int j = 0; prods[i].rhs[j]; j++) {
            if (prods[i].rhs[j] != '#' && !used[(int)prods[i].rhs[j]]) {
                symbols[(*sym_count)++] = prods[i].rhs[j];
                used[(int)prods[i].rhs[j]] = 1;
            }
        }
    }
}

int char_in_set(char c, char set[], int count) {
    for (int i = 0; i < count; i++) {
        if (set[i] == c) return 1;
    }
    return 0;
}

void add_to_first(char non_term, char symbol) {
    int idx = non_term - 'A';
    if (!char_in_set(symbol, first[idx], first_count[idx])) {
        first[idx][first_count[idx]++] = symbol;
    }
}

void add_to_follow(char non_term, char symbol) {
    int idx = non_term - 'A';
    if (!char_in_set(symbol, follow[idx], follow_count[idx])) {
        follow[idx][follow_count[idx]++] = symbol;
    }
}

void compute_first() {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        first_count[i] = 0;
    }
    
    int changed = 1;
    while (changed) {
        changed = 0;
        
        for (int i = 0; i < prod_count; i++) {
            if (strcmp(prods[i].lhs, "S'") == 0) continue;
            
            char lhs = prods[i].lhs[0];
            if (!is_non_terminal(lhs)) continue;
            
            int lhs_idx = lhs - 'A';
            int old_count = first_count[lhs_idx];
            
            char first_sym = prods[i].rhs[0];
            
            if (first_sym == '#' || !is_non_terminal(first_sym)) {
                add_to_first(lhs, first_sym);
            } else {
                int rhs_idx = first_sym - 'A';
                for (int j = 0; j < first_count[rhs_idx]; j++) {
                    add_to_first(lhs, first[rhs_idx][j]);
                }
            }
            
            if (first_count[lhs_idx] != old_count) {
                changed = 1;
            }
        }
    }
}

void compute_follow() {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        follow_count[i] = 0;
    }
    
    char start_sym = prods[1].lhs[0]; 
    add_to_follow(start_sym, '$');
    
    int changed = 1;
    while (changed) {
        changed = 0;
        
        for (int i = 0; i < prod_count; i++) {
            if (strcmp(prods[i].lhs, "S'") == 0) continue;
            
            char lhs = prods[i].lhs[0];
            if (!is_non_terminal(lhs)) continue;
            
            for (int j = 0; prods[i].rhs[j]; j++) {
                char cur = prods[i].rhs[j];
                
                if (!is_non_terminal(cur)) continue;
                
                int cur_idx = cur - 'A';
                int old_count = follow_count[cur_idx];
                
                if (prods[i].rhs[j + 1]) {
                    char next = prods[i].rhs[j + 1];
                    
                    if (is_non_terminal(next)) {
                        int next_idx = next - 'A';
                        for (int k = 0; k < first_count[next_idx]; k++) {
                            if (first[next_idx][k] != '#') {
                                add_to_follow(cur, first[next_idx][k]);
                            }
                        }
                        
                        if (char_in_set('#', first[next_idx], first_count[next_idx])) {
                            int lhs_idx = lhs - 'A';
                            for (int k = 0; k < follow_count[lhs_idx]; k++) {
                                add_to_follow(cur, follow[lhs_idx][k]);
                            }
                        }
                    } else {
                        add_to_follow(cur, next);
                    }
                } else {
                    int lhs_idx = lhs - 'A';
                    for (int k = 0; k < follow_count[lhs_idx]; k++) {
                        add_to_follow(cur, follow[lhs_idx][k]);
                    }
                }
                
                if (follow_count[cur_idx] != old_count) {
                    changed = 1;
                }
            }
        }
    }
}

void display_first_follow() {
    printf("\nFIRST Sets:\n");
    for (int i = 0; i < prod_count; i++) {
        if (strcmp(prods[i].lhs, "S'") == 0) continue;
        
        char lhs = prods[i].lhs[0];
        if (!is_non_terminal(lhs)) continue;
        
        int idx = lhs - 'A';
        
        int already_printed = 0;
        for (int j = 0; j < i; j++) {
            if (strcmp(prods[j].lhs, "S'") != 0 && prods[j].lhs[0] == lhs) {
                already_printed = 1;
                break;
            }
        }
        
        if (!already_printed && first_count[idx] > 0) {
            printf("FIRST(%c) = { ", lhs);
            for (int j = 0; j < first_count[idx]; j++) {
                if (first[idx][j] == '#') {
                    printf("ε");
                } else {
                    printf("%c", first[idx][j]);
                }
                if (j < first_count[idx] - 1) printf(", ");
            }
            printf(" }\n");
        }
    }
    
    printf("\nFOLLOW Sets:\n");
    for (int i = 0; i < prod_count; i++) {
        if (strcmp(prods[i].lhs, "S'") == 0) continue;
        
        char lhs = prods[i].lhs[0];
        if (!is_non_terminal(lhs)) continue;
        
        int idx = lhs - 'A';
        
        int already_printed = 0;
        for (int j = 0; j < i; j++) {
            if (strcmp(prods[j].lhs, "S'") != 0 && prods[j].lhs[0] == lhs) {
                already_printed = 1;
                break;
            }
        }
        
        if (!already_printed && follow_count[idx] > 0) {
            printf("FOLLOW(%c) = { ", lhs);
            for (int j = 0; j < follow_count[idx]; j++) {
                printf("%c", follow[idx][j]);
                if (j < follow_count[idx] - 1) printf(", ");
            }
            printf(" }\n");
        }
    }
}

void get_terminals_and_nonterminals() {
    terminal_count = 0;
    non_terminal_count = 0;
    int used_term[256] = {0};
    int used_nonterm[256] = {0};
    
    for (int i = 0; i < prod_count; i++) {
        if (strcmp(prods[i].lhs, "S'") != 0) {
            char lhs = prods[i].lhs[0];
            if (is_non_terminal(lhs) && !used_nonterm[(int)lhs]) {
                non_terminals[non_terminal_count++] = lhs;
                used_nonterm[(int)lhs] = 1;
            }
        }
        
        for (int j = 0; prods[i].rhs[j]; j++) {
            char sym = prods[i].rhs[j];
            if (sym == '#') continue;
            
            if (is_non_terminal(sym)) {
                if (!used_nonterm[(int)sym]) {
                    non_terminals[non_terminal_count++] = sym;
                    used_nonterm[(int)sym] = 1;
                }
            } else {
                if (!used_term[(int)sym]) {
                    terminals[terminal_count++] = sym;
                    used_term[(int)sym] = 1;
                }
            }
        }
    }
    
    if (!used_term[(int)'$']) {
        terminals[terminal_count++] = '$';
    }
}

void construct_parsing_table() {
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < 256; j++) {
            parse_table[i].action[j].action_type = ACTION_ERROR;
            parse_table[i].action[j].value = -1;
        }
        for (int j = 0; j < 26; j++) {
            parse_table[i].goto_table[j] = -1;
        }
    }
    
    for (int i = 0; i < state_count; i++) {
        ItemSet *state = &states[i];
        
        for (int j = 0; j < state->count; j++) {
            Item item = state->items[j];
            char next_sym = get_symbol_after_dot(item.prod_num, item.dot_pos);
            
            if (next_sym == '\0') {
                if (item.prod_num == 0) {
                    parse_table[i].action['$'].action_type = ACTION_ACCEPT;
                    parse_table[i].action['$'].value = 0;
                } else {
                    for (int k = 0; k < terminal_count; k++) {
                        char term = terminals[k];
                        parse_table[i].action[(int)term].action_type = ACTION_REDUCE;
                        parse_table[i].action[(int)term].value = item.prod_num;
                    }
                }
            }
        }
        
        for (int j = 0; j < trans_count; j++) {
            if (transitions[j].from_state == i) {
                char sym = transitions[j].symbol;
                int to_state = transitions[j].to_state;
                
                if (is_non_terminal(sym)) {
                    parse_table[i].goto_table[sym - 'A'] = to_state;
                } else {
                    parse_table[i].action[(int)sym].action_type = ACTION_SHIFT;
                    parse_table[i].action[(int)sym].value = to_state;
                }
            }
        }
    }
}

void display_parsing_table() {
    printf("\n========================================\n");
    printf("LR(0) PARSING TABLE\n");
    printf("========================================\n\n");
    
    printf("%-8s", "State");
    
    printf("| %-40s", "ACTION");
    
    printf("| %-20s", "GOTO");
    printf("\n");
    
    printf("%-8s", "");
    printf("|");
    for (int i = 0; i < terminal_count; i++) {
        printf(" %-5c", terminals[i]);
    }
    printf(" |");
    for (int i = 0; i < non_terminal_count; i++) {
        printf(" %-3c", non_terminals[i]);
    }
    printf("\n");
    
    for (int i = 0; i < 80; i++) printf("-");
    printf("\n");
    
    for (int i = 0; i < state_count; i++) {
        printf("%-8d", i);
        printf("|");
        
        for (int j = 0; j < terminal_count; j++) {
            char term = terminals[j];
            ActionEntry entry = parse_table[i].action[(int)term];
            
            if (entry.action_type == ACTION_SHIFT) {
                printf(" s%-4d", entry.value);
            } else if (entry.action_type == ACTION_REDUCE) {
                printf(" r%-4d", entry.value);
            } else if (entry.action_type == ACTION_ACCEPT) {
                printf(" %-5s", "acc");
            } else {
                printf(" %-5s", "");
            }
        }
        
        printf(" |");
        
        for (int j = 0; j < non_terminal_count; j++) {
            char nt = non_terminals[j];
            int goto_state = parse_table[i].goto_table[nt - 'A'];
            
            if (goto_state >= 0) {
                printf(" %-3d", goto_state);
            } else {
                printf(" %-3s", "");
            }
        }
        
        printf("\n");
    }
    
    printf("\n");
    printf("Legend: s = shift, r = reduce, acc = accept\n");
}

void print_item(Item item) {
    printf("%s -> ", prods[item.prod_num].lhs);
    int pos = 0;
    for (int i = 0; prods[item.prod_num].rhs[i]; i++) {
        if (pos == item.dot_pos) {
            printf(". ");
        }
        printf("%c ", prods[item.prod_num].rhs[i]);
        pos++;
    }
    if (pos == item.dot_pos) {
        printf(".");
    }
    printf("\n");
}

void print_item_set(ItemSet *set) {
    printf("I%d :\n", set->state_num);
    for (int i = 0; i < set->count; i++) {
        printf("    ");
        print_item(set->items[i]);
    }
}

void construct_canonical_collection() {
    ItemSet initial;
    initial.count = 0;
    initial.state_num = 0;
    
    Item start_item = {0, 0};
    add_item(&initial, start_item);
    closure(&initial);
    
    states[state_count++] = initial;
    
    int i = 0;
    while (i < state_count) {
        char symbols[MAX_SYMBOLS];
        int sym_count;
        get_all_symbols(symbols, &sym_count);
        
        for (int j = 0; j < sym_count; j++) {
            ItemSet next;
            next.count = 0;
            goto_set(&states[i], &next, symbols[j]);
            
            if (next.count > 0) {
                int existing = find_state(&next);
                
                if (existing == -1) {
                    next.state_num = state_count;
                    states[state_count++] = next;
                    
                    Transition t = {i, next.state_num, symbols[j]};
                    transitions[trans_count++] = t;
                } else {
                    Transition t = {i, existing, symbols[j]};
                    int t_exists = 0;
                    for (int k = 0; k < trans_count; k++) {
                        if (transitions[k].from_state == t.from_state &&
                            transitions[k].to_state == t.to_state &&
                            transitions[k].symbol == t.symbol) {
                            t_exists = 1;
                            break;
                        }
                    }
                    if (!t_exists) {
                        transitions[trans_count++] = t;
                    }
                }
            }
        }
        i++;
    }
}

void display_results() {
    compute_first();
    compute_follow();
    
    display_first_follow();
    
    printf("\nAugmented Grammar :\n");
    for (int i = 0; i < prod_count; i++) {
        printf("%d. %s -> %s\n", i, prods[i].lhs, prods[i].rhs);
    }
    
    printf("\n----------------------------------------\n");
    
    for (int i = 0; i < state_count; i++) {
        print_item_set(&states[i]);
        
        for (int j = 0; j < trans_count; j++) {
            if (transitions[j].from_state == i) {
                printf("GOTO ( I%d , %c ) = I%d\n", i, 
                       transitions[j].symbol, transitions[j].to_state);
            }
        }
        
        if (i < state_count - 1) {
            printf("\n");
        }
    }
    
    printf("----------------------------------------\n");
    printf("Canonical Collection of LR(0) Item Sets :\n");
    for (int i = 0; i < state_count; i++) {
        printf("I%d", i);
        if (i < state_count - 1) printf(" , ");
    }
    printf("\n");
    
    get_terminals_and_nonterminals();
    
    construct_parsing_table();
    display_parsing_table();
}

int main() {
    int n;
    char line[100], lhs[10], rhs[100];
    
    printf("Enter number of productions : ");
    scanf("%d", &n);
    getchar();
    
    printf("Enter productions :\n");
    
    for (int i = 0; i < n; i++) {
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;
        
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            strcpy(lhs, line);
            strcpy(rhs, eq + 1);
            trim(lhs);
            trim(rhs);
            
            int is_first = 1;
            char *alt = strtok(rhs, "|");
            while (alt) {
                trim(alt);
                if (i == 0 && is_first) {
                    strcpy(prods[prod_count].lhs, "S'");
                    prods[prod_count].rhs[0] = lhs[0];
                    prods[prod_count].rhs[1] = '\0';
                    prod_count++;
                }
                
                strcpy(prods[prod_count].lhs, lhs);
                strcpy(prods[prod_count].rhs, alt);
                prod_count++;
                
                is_first = 0;
                alt = strtok(NULL, "|");
            }
        }
    }
    
    construct_canonical_collection();
    display_results();
    
    return 0;
}
