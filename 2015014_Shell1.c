#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

char *command_array[100];

int pipecount(char* string1)				//count pipes
{
int i, count=0;
    for (i=0;i<strlen(string1);i++)
        if (string1[i]=='|')
            count++;
    return count;
}
void handlepipe(char* args[], int pipes) {			//piping
    const int commands = pipes + 1;
    int i = 0;
    int no_pipe_fd[2*pipes];
    for(i = 0; i < pipes; i++){
        if(pipe(no_pipe_fd + i*2) < 0) {
            exit(1);
        }
    }

    int pid,status,cursor;
    int j = 0,k=0,s=1;
    int cmd_start[10];
    cmd_start[0] = 0;
    while (args[k] != NULL){
        if(!strcmp(args[k], "|")){
            args[k] = NULL;
            cmd_start[s] = k+1;
            s++;
        }
        k++;
    }
    for (i = 0; i < commands; ++i) {
        cursor = cmd_start[i];

        pid = fork();
        if(pid == 0) {
            if(i < pipes){
                if(dup2(no_pipe_fd[j + 1], 1) < 0){
                    perror("dup2");
                    exit(1);
                }
            }
            if(j != 0 ){
                if(dup2(no_pipe_fd[j-2], 0) < 0){
                    perror("dup2");
                    exit(1);
                }
            }
            int q;
            for(q = 0; q < 2*pipes; q++){
                    close(no_pipe_fd[q]);
            }
            if( execvp(args[cursor], args+cursor) < 0 ){
                    perror(*args);
                    exit(1);
            }
        }
        else if(pid < 0){
            perror("error");
            exit(1);
        }

        j+=2;
    }

    for(i = 0; i < 2 * pipes; i++){
        close(no_pipe_fd[i]);
    }

    for(i = 0; i < pipes + 1; i++){
        wait(&status);
    }
    }


void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    printf("\n Type exit to quit.\n");
    printf(">");
    fflush(stdout);
}

int main()
{
  char complete_path[20];	//full file path
  char path[5] = "/bin/";
  char cmd[20];
  char *token;
  char *argument_array[100];	//user command
  char argumentzero[20];
  int status = 1;
  printf ("Welcome to my shell.\n");
  while (1)
    {
      signal(SIGINT, sigintHandler);
      printf ("> ");
      gets (cmd);
      //printf("no of pipes= %d",pipecount(cmd));
    if (pipecount(cmd)>0)
    {
    int x1=pipecount(cmd);
    int l=0;
    //printf("in func");
    char* array1[100];
    int x;
    for (x=0;x<100;x++)
        array1[x]=NULL;
    char* tkn;
    tkn=strtok(cmd," ");
    while (tkn != NULL)
	{
	  array1[l] = tkn;
	  tkn = strtok (NULL, " ");
	  l++;
	}
    handlepipe(array1,x1);
    //printf("here %d",pipecount(cmd));
    }
      FILE *commandptr;
        commandptr = fopen("command.txt", "a");
        fprintf(commandptr,"%s\n",cmd);
        fclose(commandptr);
      token = strtok (cmd, " ");
      int i = 0;
      if(strcmp(cmd,"history")==0)
				{
				//show contents of file
                    char c;
                    FILE* readpointer=fopen("command.txt", "r");
					c = fgetc(readpointer);
                    while (c != EOF)
                    {
                        printf ("%c", c);
                        c = fgetc(readpointer);
                    }
					fclose(readpointer);
					continue;
				}
      if (strcmp (cmd, "exit") == 0 || strcmp(cmd, "EXIT")==0)
	{
	  break;
	}


      while (token != NULL)
	{
	  argument_array[i] = token;
	  if (i == 0)
	    strcpy (argumentzero, token);
	  token = strtok (NULL, " ");
	  i++;
	}
      argument_array[i] = NULL;
      strcpy (complete_path, path);
      strcat (complete_path, argumentzero);
      if(strcmp(argumentzero,"cd")==0)
    {
        chdir(argument_array[1]);
        continue;
    }


//printf("full path is%s",complete_path);
      int pid = fork ();
      if (pid == 0)
	{

		if (execvp(complete_path, argument_array)< 0)
        printf("Error! Command not found.\n");
	  execvp (complete_path, argument_array);
	  exit (1);
	}
      else
    wait (NULL);
    }
  return 0;
}
