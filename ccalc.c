#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* STRUCTS AND ENUMS */
enum Operation {
        OP_ADD,
        OP_SUBTRACT,
        OP_MULTIPLY,
        OP_DIVIDE,
        OP_ERR,
};

enum DefineResult {
        OP_OK,
        OP_META,
        OP_UNKNOWN,
};

enum MetaCommandResult {
        META_OK,
        META_UNKNOWN,
};

enum ExecuteResult {
        EXEC_SUCCESS,
        EXEC_FAILURE,
};

struct Statement {
        long num_1;
        long num_2;
        int op;
};

struct InputBuf {
        char *buffer;
        size_t buffer_len;
        ssize_t input_len;
};

/* INPUT BUFFER */
struct InputBuf *new_input_buf(void)
{
        struct InputBuf *buf = (struct InputBuf *)malloc(sizeof(struct InputBuf));
        buf->buffer = NULL;
        buf->buffer_len = 0;
        buf->input_len = 0;

        return buf;
}

void read_input_to_buf(struct InputBuf *buf)
{
        ssize_t bytes_read = getline(&(buf->buffer), &(buf->buffer_len), stdin);
        
        if (bytes_read <= 0) {
                printf("Error reading input to buffer.\n");
                exit(EXIT_FAILURE);
        }

        buf->input_len = bytes_read - 1;
        buf->buffer[bytes_read - 1] = 0; 
}

void close_input_buf(struct InputBuf *buf)
{
        free(buf->buffer);
        free(buf);
}

/* STATEMENT PARSING */
int define_statement(struct InputBuf *buf, struct Statement *stat)
{
        char *strptr;

        if (buf->buffer[0] == '.') {
                return OP_META;
        }

        stat->num_1 = strtol(buf->buffer, &strptr, 10);
        char *operation = strtok(strptr, " 012345689");
        stat->num_2 = strtol(buf->buffer, &strptr, 10);

        if (strcmp(operation, "+") == 0) {
                stat->op = OP_ADD;
                return OP_OK;
        }
        if (strcmp(operation, "-") == 0) {
                stat->op = OP_SUBTRACT;
                return OP_OK;
        }
        if (strcmp(operation, "*") == 0) {
                stat->op = OP_MULTIPLY;
                return OP_OK;
        }
        if (strcmp(operation, "/") == 0) {
                stat->op = OP_DIVIDE;
                return OP_OK;
        }

        return OP_UNKNOWN;
}

int run_statement(struct Statement *stat)
{
        double res;

        switch (stat->op) {
                case (OP_ADD):
                        res = stat->num_1 + stat->num_2;
                        break;
                case (OP_SUBTRACT):
                        res = stat->num_1 - stat->num_2;
                        break;
                case (OP_MULTIPLY):
                        res = stat->num_1 * stat->num_2;
                        break;
                case (OP_DIVIDE):
                        res = stat->num_1 / stat->num_2;
                        break;
                case (OP_ERR):
                        return EXEC_FAILURE;
        }

        printf("%.2f\n", res);
        return EXEC_SUCCESS;
}

int run_meta(struct InputBuf *buf)
{
        if (strcmp(buf->buffer, ".exit") == 0) {
                exit(EXIT_SUCCESS);
        }

        return META_UNKNOWN;
}

/* M A I N */
int main(void)
{
        struct InputBuf *buf = new_input_buf();
                
        for (;;) {
                printf(">> ");
                read_input_to_buf(buf);

                struct Statement stat;
                switch (define_statement(buf, &stat)) {
                        case (OP_OK):
                                break;
                        case (OP_META):
                                switch (run_meta(buf)) {
                                        case (META_OK):
                                                break;
                                        case (META_UNKNOWN):
                                                printf("Unrecognized meta command: %s\n", buf->buffer);
                                                continue;
                                }
                        case (OP_UNKNOWN):
                                printf("Unrecognized operation: %d\n", stat.op);
                                continue;
                }

                switch (run_statement(&stat)) {
                        case (EXEC_SUCCESS):
                                break;
                        case (EXEC_FAILURE):
                                printf("Error executing statement.\n");
                                exit(EXIT_FAILURE);
                }
        }
}
