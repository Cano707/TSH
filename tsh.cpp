/*
 * ===========================================================================
 *
 * tsh.cpp --
 *
 * ===========================================================================
 */


#include "makeargv.h"

#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

// maximal length of user input
#define MAX_COMMAND_LINE_LENGTH 1024

void wait_proc(int pid);
void info(int pid);
void list();
void job(char** argv_intern, char* buffer);
void execution(char **argv_intern, char* buffer);
void kill_proc(int pid);

/* type definitions **************************************************************/

// constants for job status
typedef enum {
  JOB_RUNNING, JOB_FINISHED
} job_status_t;

// job descriptor
typedef struct {
  char* command;            // c-string containing the original command
  int internal_id;          // job id (shell-internal)
  int pid;                  // process id of the job (from OS)
  int exit_status;          // exit status of the job
  job_status_t job_status;  // status of the job
} job_descr_t;


/* global variables **************************************************************/

// list of all jobs ever started in the shell;
// list elements are POINTERS to job descriptors
static std::vector<job_descr_t*> job_list;



/* functions ********************************************************************/

// function for the processing of the user input;
// shows prompt and waits for user input;
// user input is parsed and appropriate action is taken...
void
tsh_prompt_and_process()
{
  // buffer for user input
  static char buffer[MAX_COMMAND_LINE_LENGTH+1];

  char** argv_intern = NULL;  // holds the user input after separation into tokens
  int numtokens = 0;          // number of tokens

  // show prompt and wait for user input
  printf( "tsh> " ); fflush( stdout );
  fgets( buffer, MAX_COMMAND_LINE_LENGTH, stdin );
  
  // separate char string in buffer (user input) into single tokens
  numtokens = makeargv( buffer, " \n", &argv_intern );

  // ======================================================================
  // BEGIN: HERE GOES YOUR CODE! 
  // ======================================================================
  
  /*
  // TODO: REMOVE THIS FOR LOOP AFTER YOUR SOLUTION IS FINISHED
  for( int i=0; i < numtokens; i++ ) {
    printf( "%s\n", argv_intern[i] );
  }
  */

  // check if at least one token was entered at the command line
  // (if not: do nothing)
  if( numtokens > 0 )
  {
      if( strcmp( "quit", argv_intern[0] ) == 0 )
      {
        _exit( 0 );
      }
      else if( strcmp( "info", argv_intern[0] ) == 0 )
      {
        info((int)(*argv_intern[1]-'0'));
      }
      else if( strcmp( "list", argv_intern[0] ) == 0 )
      {
        list();
      }
      else if( strcmp( "wait", argv_intern[0] ) == 0 )
      {
        wait_proc(strtol(argv_intern[1], NULL, 10));
      }
      else if( strcmp( "kill", argv_intern[0] ) == 0 )
      {
        kill_proc(strtol(argv_intern[1], NULL, 10));
      }
      else if( strcmp( "job", argv_intern[0] ) == 0 )
      {
        job(argv_intern, buffer);
      }
      else
      {
        execution(argv_intern, buffer);   
      }
  }

  // ======================================================================
  // END: HERE GOES YOUR CODE! 
  // ======================================================================
  
  // clean up
  if( argv_intern )
    freemakeargv( argv_intern );

  return;
}


/* main **************************************************************************/

int
main( int argc, char **argv )
{
  // init lists
  job_list.resize(0);

  // main loop
  while( 1 ) {
    // process user input
    tsh_prompt_and_process();
  }

  _exit( 0 );
}

void job(char **argv_intern, char* buffer) {
  pid_t cpid = fork();
  if(cpid < 0) {
    perror("fork");
    _exit(EXIT_FAILURE);
  } else if(cpid == 0) {
    int status_code = execvp(argv_intern[1], &argv_intern[1]);
    if(status_code == -1) {
        printf("[command not found (or other general execution error)]\n");
        printf("[status code = 255]\n");
        return;
    }
  }
  job_descr_t* job_descr = new job_descr_t;
  job_descr->command=strdup(&buffer[3]);
  job_descr->internal_id=job_list.size();
  job_descr->pid=cpid;
  int wait_pid = waitpid(cpid, &(job_descr->exit_status), WNOHANG);
  job_descr->exit_status=WEXITSTATUS(job_descr->exit_status);
  if(wait_pid) {
    job_descr->job_status=JOB_FINISHED;  
  } else if(wait_pid == 0) {
    job_descr->job_status=JOB_RUNNING;
  } else {
    perror("waitpid");
  }
  job_list.push_back(job_descr);
}


void execution(char** argv_intern, char* buffer) {
  pid_t cpid=fork();
  if(cpid==0) {
    int status_code = execvp(argv_intern[0], argv_intern);
    if(status_code == -1) {
      printf("[command not found (or other general execution error)]\n");
      printf("[status code = 255]\n");
      _exit(1);
    }

  } else {
    job_descr_t* job_descr = new job_descr_t;
    job_descr->command=strdup(buffer);
    job_descr->internal_id=job_list.size();
    job_descr->pid=cpid;
    job_descr->job_status=JOB_FINISHED;
    waitpid(cpid, &(job_descr->exit_status), 0);
    job_descr->exit_status=WEXITSTATUS(job_descr->exit_status);
    job_list.push_back(job_descr);
  } 
}

void info(int pid) {
  for (size_t i=0; i < job_list.size(); i++) {
    if((*job_list[i]).internal_id == pid) {
      const char* status_string=NULL;
      int status = waitpid(job_list[i]->pid, &(job_list[i]->exit_status), WNOHANG);
      job_list[i]->exit_status = WEXITSTATUS(job_list[i]->exit_status);
      if(status) {
        job_list[i]->job_status=JOB_FINISHED;
        status_string = "finished_status";
      } else if(status<0) {
        perror("waitpid");
      } else {
        status_string = "running_status";
      }
      printf("%d (pid %d %s=%d): %s\n", job_list[i]->internal_id, job_list[i]->pid, status_string, job_list[i]->exit_status, job_list[i]->command);
    }
  }
}

void list() {
  for (size_t i=0; i < job_list.size(); i++) {
    const char* status_string=NULL;
    int status = waitpid(job_list[i]->pid, &(job_list[i]->exit_status), WNOHANG);
    job_list[i]->exit_status = WEXITSTATUS(job_list[i]->exit_status);
    if(status) {
      job_list[i]->job_status=JOB_FINISHED;
      status_string = "finished_status";
    } else if(status<0) {
      perror("waitpid");
    } else {
      status_string = "running_status";
    }
    printf("%d (pid %d %s=%d): %s\n", job_list[i]->internal_id, job_list[i]->pid, status_string, job_list[i]->exit_status, job_list[i]->command);
  }
}

void kill_proc(int pid) {
  for(int i = 0; i < (int)job_list.size(); i++) {
    if(job_list[i]->internal_id == pid) {
      if(kill(job_list[i]->pid, SIGKILL) == -1) {
        perror("kill");
      } else {
        waitpid(job_list[i]->pid, &(job_list[i]->exit_status), 0);
        job_list[i]->job_status=JOB_FINISHED;
      }
    }
  }
}

void wait_proc(int pid) {
  for(int i = 0; i < (int)job_list.size(); i++) {
    if(job_list[i]->internal_id == pid) {
      waitpid(job_list[i]->pid, &(job_list[i])->exit_status, 0);
      job_list[i]->exit_status=WEXITSTATUS(job_list[i]->exit_status);
      job_list[i]->job_status=JOB_FINISHED;
    }
  }
}