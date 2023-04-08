#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

// FUNCTION DECLARATIONS FOR BUILT IN SHELLS
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

// LIST OF BUILT IN COMMANDS FOLLOWED BY THEIR CORRESPONDING FUNCTIONS
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

// BUILTIN FUNCTION IMPLEMENTATION.
int lsh_cd(char **args){
    if (args[1] == NULL){
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    }
    else{
        if (chdir(args[1] != 0)){
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args){
    int i;
    printf("Sahil Prabhu's LSH\n");
    printf("Type program name and arguments > \n");
    printf("The following are built in > \n");
    for (i = 0;i < lsh_num_builtins();i++){
        printf(" %s\n",builtin_str[i]);
    }
    printf("Use \"man\" for information on other programs\n");
    return 1;
}

int lsh_exit(char **args){
    return 0;
}

int lsh_launch(char **args){
    pid_t pid,wpid;
    int status;
    pid = fork();
    if (pid == 0){
        // CHILD PROCESS
        if (execvp(args[0],args) == -1){
            perror("lsh");
        }
    exit(EXIT_FAILURE);
    }
    else if (pid < 0){
        // ERROR FORKING
        perror("lsh");
    }
    else{
        // PARENT PROCESS
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        }while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int lsh_execute(char **args){
    int i;
    if (args[0] == NULL){
        // AN EMPTY COMMAND WAS ENTERED.
        return 1;
    }
    for (i = 0;i < lsh_num_builtins();i++){
        if (strcmp(args[0],builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}

char **lsh_split_line(char *line){
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens){
        fprintf(stderr, "lsh: allocataion error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL){
        tokens[position] = token;
        position++;

        if (position >= bufsize){
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens){
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

char *lsh_read_line(void){
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer){
        fprintf(stderr,"lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1){
        // READ A CHARACTER
        c = getchar();

        // IF WE HIT EOF, REPLACE IT WITH A NULL CHARACTER AND RETURN.
        if (c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }
        else {
            buffer[position] = c;
        }
        position++;

        //IF WE HAVE EXCEEDED THE BUFFER, RELOCATE.
        if (position >= bufsize){
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer,bufsize);
            if (!buffer){
                fprintf(stderr,"lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void lsh_loop(void){
    char *line;
    char **args;
    int status;

    do{
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);
        free(line);
        free(args);
    }while(status);
}

int main(int argc, char **argv){
    // LOAD CONFIG FILES< IF ANY.

    // RUN COMMAND LOOP.
    lsh_loop();

    // PERFORM SHUTDOWN OR CLEANUP.
    return EXIT_SUCCESS;
}