#include "util.h"

void dfsmat(Transition* t, int s, int mat[][256]) {
    if (t != NULL) {
        dfsmat(t->left,s,mat);
        mat[s][t->ch] = t->to;
        dfsmat(t->right,s,mat);
    }
}


void dfa2matrix(DFA* dfa) {
    int matrix[dfa->numstates+1][256];
    for (int i = 0; i < dfa->numstates+1; i++)
        for (int j = 0; j < 256; j++)
            matrix[i][j] = 0;
    for (int i = 1; i <= dfa->numstates; i++) {
        dfsmat(dfa->states[i]->transitions, i, matrix);
    }
    for (int i = 0; i < 128; i++) printf("%c ", i <= 30 ? ' ':(char)i);
    printf("\n");
    for (int i = 1; i <= dfa->numstates; i++) {
        for (int j = 0; j < 128; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}


void dfa2json(DFA* dfa) {
    FILE* fd = fopen("table_gen.trans", "w+");
    fprintf(fd, "{\n  \"DFA\": [\n");
    int i;
    for (i = 1; i < dfa->numstates; i++) {
        serialize_dfa_state(dfa->states[i], i, fd);
        fprintf(fd, ",\n");
    }
    serialize_dfa_state(dfa->states[i], i, fd);
    fprintf(fd, "\n\t]\n}\n");
    fclose(fd);
}

void serialize_dfa_state(DFAState* state, int from, FILE* fd) {
    if (state == NULL)
        return;
    Transition* st[255];
    int stsp = 0;
    int tc = 0;
    fprintf(fd, "\t\t{\n\t\t\t\"state\": %d,\n\t\t\t\"accepting\": %s,\n\t\t\t\"transitions\":  [", from, state->is_accepting ? "true":"false");
    Transition* it = state->transitions;
    while (it != NULL) {
        st[++stsp] = it;
        it = it->left;
    }
    if (stsp > 0) fprintf(fd, "\n");
    while (stsp > 0) {
        it = st[stsp--];
        if (it != NULL) {
            fprintf(fd, "\t\t\t\t{ \"symbol\": \"%c\", \"destination\": %d }", it->ch, it->to);
            tc++;
            it = it->right;
            while (it != NULL) {
                st[++stsp] = it;
                it = it->left;
            }
        }
        if (stsp > 0) fprintf(fd, ",\n");
    }
    fprintf(fd, " ]\n\t\t}");
}

char *slurp_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(sizeof(char)*(file_size + 1));
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}