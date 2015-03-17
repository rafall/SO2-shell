#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		/* fork() */
#include <sys/types.h>		/* wait() */
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_COMMAND_LINE_SIZE 1024
#define MAX_NUMBER_OF_ARGUMENTS 20


/* Struct built to help read the command line the user has entered */
typedef {
    char full[MAX_COMMAND_LINE_SIZE];
    char** spaceTok;
    char** barTok;
} commandLine;

commandLine command;

/* Global variable that counts the number of commands the user has entered */
int command_count;

int main (int argc, char **argv)
{
    // Number of ??
    int count = 1;
    
    // Process ID
    int pid;

    while(count) /* main loop */
    {
        /*  */
        printf("\n$ ");
        fgets(command.full, MAX_COMMAND_LINE_SIZE, stdin);
        if(0 < strlen(command) && '\n' != command[0])
        {
            command[strlen(command) - 1] = '\0';
        }

        command_count = separateStringByBar(command.full, command.spaceTok);

        execute();
    }
    printf("Bye Byee\n");
    return EXIT_SUCCESS;
}

int separateStringBySpace(char* line, char** buffer) /* Breaks de command string */
{
    int i;
    char* ch;
    buffer = (char**) malloc (MAX_NUMBER_OF_ARGUMENTS*sizeof(char*));

    ch = strtok(line, " ");
    i = 0;

    while (ch != NULL)
    {
        buffer[i] = (char *) malloc (strlen(ch)*sizeof(char));
        strcpy(buffer[i], ch);
        ch = strtok(NULL, " ");
        i++;
    }
    buffer[i] = NULL;

    return i;
}

int separateStringByBar(char* line, char** buffer) /* Breaks de command string */
{
    int i;
    char* ch;
    buffer = (char**) malloc (MAX_NUMBER_OF_ARGUMENTS*sizeof(char*));

    ch = strtok(line, "|");
    i = 0;

    while (ch != NULL)
    {
        buffer[i] = (char *) malloc (strlen(ch)*sizeof(char));
        strcpy(buffer[i], ch);
        ch = strtok(NULL, "|");
        i++;
    }
    buffer[i] = NULL;

    return i;
}

bool execute(char* line) /* Execute the command line given by the user */
{
    int current_command = command_count;
    separateStringBySpace(line, command.spaceTok);
    if(command_count >= 0)
    {
        command_count--;
        execute(command.spaceTok[command_count]);
    }

    pid = fork(); /* Fork */

    if (pid < 0)
    {
        fatal();
    }

    if (pid > 0)
    {
        wait(&status);
    }
    else
    {
        if(current_command == 0)
        {
            close(1); /* close normal stdout */
            dup(fd[0][1]); /* make stdout same as fd[0][1] */
            close(fd[0][0]);
        }
        else if(current_command == numOfCommands)
        {
            close(0); /* close normal stdin */
            dup(fd[numOfCommands - 1][0]); /* make stdin same as fd[numOfCommands -1][0] */
            close(fd[numOfCommands -1][1]);
        }
        else
        {
            close(0); /* close normal stdin */
            dup(fd[current_command - 1][0]); /* make stdin same as fd[numOfCommands -1][0] */
            close(fd[current_command - 1][1]);
            close(1); /* close normal stdout */
            dup(fd[current_command - 1][1]); /* make stdout same as fd[0][1] */
            close(fd[0][0]);
        }
        if(!exec_builtin(command.spaceTok))
        execvp(command.spaceTok[0], command.spaceTok);
        printf("%s command not found.\n", command.spaceTok[0]);
    }
}

/* Built in function. */


/* Return 1 if command is executed as a builtin function;
   0 otherwise. */

int exec_builtin (char **line)
{

    if (!strcmp (line[0], "cd"))
    {
        return cd(line[1]);
    }
    if (!strcmp (line[0], "help"))
    {
        return help();
    }
    if (!strcmp (line[0], "exit"))
    {
        cont = 0;
        return 1;
    }

    return 0;
}


/* Command cd */
int cd (char *path)
{
    char cwd[300];
    if(path == NULL) /* If there is no argument the directory is changed to home */
    {
        if(!chdir("/home"))
            return 1;
        else
            return 0;
    }
    else
    {
        getcwd(cwd, sizeof(cwd));
        strcat(cwd, "/");
        strcat(cwd, path);

        if(!chdir(cwd))
            return 1; /* Success! The working directory is changed */
        else
            return 0; /* Fail! The working directory was not changed */
    }
}

void help ()
{
    printf("Hi! You asked for help but I know as much as you do.\nSorry :)\n");
}
