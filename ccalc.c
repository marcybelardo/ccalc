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
        char *num_1;
        char *num_2;
        char *op;
};

struct Values {
        double num_1;
        double num_2;
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
int parse_tokens(char *str, struct Statement *stat)
{
        stat->num_1 = strtok(str, " +-*/");
        // printf("num1 \t'%s'\n", stat->num_1);

        stat->op = strtok(NULL, " 0123456789");
        // printf("op  \t'%s'\n", stat->op);

        stat->num_2 = strtok(NULL, " ");
        // printf("num2 \t'%s'\n", stat->num_2);

        return 0;
}

int read_statement_to_struct(struct InputBuf *buf, struct Statement *stat, struct Values *vals)
{
        if (buf->buffer[0] == '.') {
                return OP_META;
        }

        if (parse_tokens(buf->buffer, stat) != 0) {
                return -1;
        }

        switch (*stat->op) {
                case ('+'):
                        vals->op = OP_ADD;
                        return OP_OK;
                case ('-'):
                        vals->op = OP_SUBTRACT;
                        return OP_OK;
                case ('*'):
                        vals->op = OP_MULTIPLY;
                        return OP_OK;
                case ('/'):
                        vals->op = OP_DIVIDE;
                        return OP_OK;
        }

        return OP_UNKNOWN;
}

void convert_data_to_doubles(struct Statement *stat, struct Values *vals)
{
        vals->num_1 = strtod(stat->num_1, NULL);
        vals->num_2 = strtod(stat->num_2, NULL);
}

int run_statement(struct Values *vals)
{
        double res;

        switch (vals->op) {
                case (OP_ADD):
                        res = vals->num_1 + vals->num_2;
                        break;
                case (OP_SUBTRACT):
                        res = vals->num_1 - vals->num_2;
                        break;
                case (OP_MULTIPLY):
                        res = vals->num_1 * vals->num_2;
                        break;
                case (OP_DIVIDE):
                        res = vals->num_1 / vals->num_2;
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
        for (;;) {
                struct InputBuf *buf = new_input_buf();
                printf(">> ");
                read_input_to_buf(buf);

                struct Statement stat;
                struct Values vals;
                switch (read_statement_to_struct(buf, &stat, &vals)) {
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
                                printf("Unrecognized operation: %s\n", stat.op);
                                continue;
                }

                convert_data_to_doubles(&stat, &vals);
                switch (run_statement(&vals)) {
                        case (EXEC_SUCCESS):
                                break;
                        case (EXEC_FAILURE):
                                printf("Error executing statement.\n");
                                exit(EXIT_FAILURE);
                }

                close_input_buf(buf);
        }
}
