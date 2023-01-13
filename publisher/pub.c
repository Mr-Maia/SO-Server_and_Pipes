
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "../utils/lib.h"

int server_pipe;
int session_id;
char pipe_file[PIPE_PATH_MAX_SIZE];

int start_server_connection(char const *server_pipe_path, char const *pipe_path, char const *box_name) {

	/* Create client pipe */
	if (unlink(pipe_path) == -1 && errno != ENOENT) {
		return -1;
	}
	if (mkfifo(pipe_path, 0640) == -1) {
		return -1;
	}

	/* Open server pipe */
	if ((server_pipe = open(server_pipe_path, O_WRONLY)) == -1) {
		return -1;
	}

	/* Send request to server */
	task task_op;
	task_op.opcode = OP_CODE_LOGIN_PUB;
	strcpy(task_op.pipe_path, pipe_path);
    strcpy(task_op.box_name, box_name);
    task_op.user_type = OP_CODE_PUB;
	if (write(server_pipe, &task_op, sizeof(task)) == -1) {
		return -1;
	}
    return 0;
}



int main(int argc, char **argv) {

    
	if (argc != 4){
		printf("The number of arguments is invalid.\n");
		printf("The correct usage is: pub <register_pipe_name> <pipe_name> <box_name>\n");
		return -1;
	} 
    char *register_pipe_name = argv[1];
    char *pipe_name = argv[2];
    char *box_name = argv[3];

    server_pipe = open(register_pipe_name, O_RDONLY);
    
    // Creates a pipe and verifies if already exists
    if (mkfifo(pipe_name, 0640) == -1 && errno == EEXIST){
        fprintf(stderr, "Named Pipe already exists.");
        return -1;
    }
    fprintf(stderr, "usage: pub %s %s %s\n", register_pipe_name, pipe_name, box_name);


    //O PIPE SÓ VAI DESBLOQUEAR QUANDO FOR FEITO OPEN ONDE?


    if(start_server_connection(register_pipe_name, pipe_name, box_name) != 0){
        return -1;
    }
    //ONDE MANDO O OP CODE OU COMO LEIO O OP CODE, SEMPRE Q ESCREVO UMA MENSAGEM MANDO UM OPCODE?-->PERGUNTAR AO STOR
    int pipe_fhandle = open(pipe_name, O_WRONLY);
    
    char buffer[1], buffer2[1024];

    memset(buffer2,0,sizeof(buffer));
    int i = 0;

    while(read(STDIN_FILENO, &buffer, 1) > 0)
    {
        if(strcmp(buffer, "\n")) break;
        memset(buffer2 + i, strlen(buffer), 1);
        i++;
    } 

    write(pipe_fhandle, buffer2, sizeof(buffer2)); 


    return 0;
}
