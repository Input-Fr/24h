#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../hash_map/hash_map.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"

// static void var_field(char *str, size_t *i)
//{
//     if (str[*i] == '^' && str[*i + 1] == '^')
//     {
//         *i += 1;
//         while (*i + 1 < strlen(str) && !(str[*i] == '^' && str[*i + 1] ==
//         '^'))
//         {
//             *i += 1;
//         }
//         *i += 1;
//     }
// }

int test_cmd_sub(char *str)
{
    size_t i = 0;
    size_t quote = 0;
    while (i < strlen(str) && str[i] != '\0')
    {
        if (str[i] == '\'' && !quote)
            quote = 1;
        else if (str[i] == '\'' && quote)
            quote = 0;
        if (((str[0] == '$' || (i > 0 && str[i - 1] != '\\' && str[i] == '$'))
             && !quote))
        {
            i += 1;
            char c = str[i];
            if (c != '\0' && c == '(')
            {
                if (str[i + 1] == '(')
                {
                    return 0;
                }
                while (str[i] != '\0' && str[i] != ')')
                {
                    i += 1;
                }
                if (str[i] == ')')
                {
                    return 1;
                }
            }
        }
        i += 1;
    }
    return 0;
}

void clean_input(char *input)
{
    for (size_t i = 2; i < strlen(input) - 1; i++)
    {
        input[i - 2] = input[i];
    }
    input[strlen(input) - 3] = 0;
}

int launch_subshell(char *sub_cmd)
{
    FILE *value = fmemopen(sub_cmd, strlen(sub_cmd), "r");
    if (!value)
    {
        return -1;
    }

    // init lexer
    struct lexer *lexer = lexer_new();
    lexer->file = value;

    int ret_code = 0;
    enum parser_status status;
    struct ast *ast;
    struct hash_map *h = hash_map_init(7);
    char *bufferpwd = malloc(1024 * sizeof(char));
    while (lexer->current_tok.type != TOKEN_EOF)
    {
        ast = parse(&status, lexer);
        if (!ast)
        {
            if (status == PARSER_OK && lexer->current_tok.type != TOKEN_EOF)
            {
                continue;
            }
            if (status == PARSER_OK)
            {
                lexer_free(lexer);
                hash_map_free(h);
                free(bufferpwd);
                exit(ret_code);
            }
            else
            {
                errx(2, "Wrong grammar");
            }
        }
        h->ret = ret_code;
        h->old_pwd = getcwd(bufferpwd, 1024);
        ret_code = (*ast->ftable->run)(ast, h);

        (*ast->ftable->free)(ast);
    }

    lexer_free(lexer);
    hash_map_free(h);
    free(bufferpwd);

    return ret_code;
}

static char *expand_sub_cmd(char *sub_cmd)
{
    int pipefd[2]; // 0 read side ; 1 write side
    if (pipe(pipefd) == -1)
        errx(1, "Failed to create pipe file descriptors.");

    pid_t pid = fork();
    if (pid < 0)
        errx(1, "Failed to fork.");

    if (!pid) // child process
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        // clean_input(sub_cmd);
        int res = launch_subshell(sub_cmd);
        close(pipefd[1]);
        exit(res);
    }
    else
    {
        close(pipefd[1]);
        int wstatus;
        waitpid(pid, &wstatus, 0);
        int return_value = WEXITSTATUS(wstatus);
        // déclencher une erreur si besoin
        if (return_value)
        {
            errx(return_value, "erreur while executing command substitution\n");
        }
        size_t size = 1024;
        char *out = calloc(size, 1);
        size_t nb_bytes_read = 0;
        size_t idx = 0;
        nb_bytes_read = read(pipefd[0], out, 1024);
        while (nb_bytes_read)
        {
            idx += nb_bytes_read;
            size += nb_bytes_read;
            out = realloc(out, size);
            nb_bytes_read = read(pipefd[0], out + idx, 1024);
        }
        out[idx - 1] = 0;
        close(pipefd[0]);
        return out;
    }
    return NULL;
}

static char *delimite_substi(char *prev, char *next, char *word)
{
    char *tmp = word;
    prev = strcpy(prev, word);
    size_t j = 0;
    while (*word != '\0' && *word != '$')
    {
        j += 1;
        word += 1;
    }
    int contexte = 0;
    prev[j] = '\0';
    word += 2;
    char *new = calloc(1, strlen(word) + 1);
    new = strcpy(new, word);
    size_t i = 0;

    while (*word != '\0' && contexte != -1)
    {
        if (*word == '(')
        {
            contexte++;
        }
        if (*word == ')')
        {
            contexte--;
        }
        word += 1;
        i += 1;
    }

    new[i-1] = '\0';
    if (*word == '\0')
        next = "";
    else
    {
        next = strcpy(next, word + 1);
    }
    word = tmp;
    // error_var_brackets(word);
    return new;
}

void expand_substi(char *res)
{
    if (test_cmd_sub(res))
    {
        char *prev = calloc(1, strlen(res) + 1); // word before the variable
        char *next = calloc(1, strlen(res) + 1); // word after the variable
        char *substi =
            delimite_substi(prev, next, res); // divide the word in 3 words
        char *result = expand_sub_cmd(substi);
        strcpy(res, result);
        size_t len = strlen(prev) + strlen(result) + strlen(next) + 1;
        snprintf(res, len, "%s%s%s", prev, result, next); // concat
        free(result);
        free(next);
        free(prev);
        free(substi);
    }
}
