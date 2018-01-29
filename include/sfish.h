#ifndef SFISH_H
#define SFISH_H


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h> /* for wait */
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include "debug.h"

/* Format Strings */
#define EXEC_NOT_FOUND "sfish: %s: command not found\n"
#define JOBS_LIST_ITEM "[%d] %s\n"
#define STRFTIME_RPRMT "%a %b %e, %I:%M%p"
#define BUILTIN_ERROR  "sfish builtin error: %s\n"
#define SYNTAX_ERROR   "sfish syntax error: %s\n"
#define EXEC_ERROR     "sfish exec error: %s\n"


struct Node
{
  int data;
  struct Node *next; // Pointer to next node in DLL
  struct Node *prev; // Pointer to previous node in DLL
};

//extern struct Node *process_list;

void print_help(char* input);
char* replace_with_tilda(char* cwd, const char* homedir);
char* create_current_directory();
int check_and_execute_builtin_requests(char* input);
char* validate_and_fetch_arguments(char* input);
void cd();
void cd_dot();
void set_current_directory(char* directory);
char* get_current_directory();
void cd_dot_dot();
void save_prev_directory(char* past_directory);
void cd_dash();
char* get_prev_directory();
void pwd(char* input);
int execute(char* input);
void cd_dir(char* input);
char** parse_arguments(char* input, char* break_point);
char* get_executable_path(char* input);
char** get_path_array();
bool find_slash(char* args);
int redirect(char* input);
int case_one(char* input, int sym_index);
int case_two(char* input, int sym_index);
int case_three(char* input, int sym_index);
int case_four(char* input, int sym_index);
char* get_executable_path(char* input);
int find_index(char** args,char* character);
int exec_redirect(char* input, char* direction);
int exec_pipe(char* input);
int array_size(char** array);
int exec_redirect_help(char* input,char* direction);
void deleteNode(struct Node **head_ref, int key);
void append(struct Node** head_ref, int new_data);
void printList(struct Node *node);
struct Node* get_lastNode(struct Node *node);
void sigintHandler(int sig_num);
void kill_child(int sig, pid_t child_pid);
void sighup(int signo);
void sigint(int signo);
void sigquit(int signo);
void sigtstp(int signo);
bool find_char(char* args, char* element);
void SIGTSTP_handle();
void SIGINT_handle();
void proc_exit();
int count_carrots(char* input);
bool is_duplicate(char* input, char* element);
int validate_pipes(char** arguments);
int substitution(char* input);

#endif
