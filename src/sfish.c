#include "sfish.h"


void print_help(char* input){
    char** arguments = parse_arguments(input, " ");
    if(array_size(arguments)>1){
        if(*arguments[1] != '>'){
            perror("sfish builtin error: command not found\n");
            return;
        }
    }
    
    if(find_char(input,">")){
        exec_redirect_help(input,">");
    }else{
        printf("%s","Shell written by Ali Hamdany\n");
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        printf("help: Prints a list of all builtin functions and their basic usage.\nexit: Exits the shell by using the exit(3) function.\ncd: Changes the current working directory of the shell.\n   cd - should change the working directory to the last directory the user was in.\n   cd with no arguments should go to the user's home directory which is stored in the HOME environment variable.\n   cd . and cd .. should be handled correctly. . and .. are special directories that appear in every directory of a Unix system. They correspond to the current directory and previous directory of the current working directory.\npwd: Prints the absolute path of the current working directory. This can be obtained by using the getcwd(3) function.\n");
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    }
}

bool is_duplicate(char* input, char* element){
    int counter = 0;
    while(*input != '\0'){
        if(*input == *element){
            counter++;
        }
        input++;
    }
    if(counter > 1){
        return true;
    }else{
        return false;
    }
}

char* create_current_directory(){
    char cwd[1024] ={0};
    char* current_directory;
    current_directory = calloc(256,sizeof(char));
    char* pwd = calloc(256,sizeof(char));
    const char* homedir = getenv("HOME");

    if (getcwd(cwd, sizeof(cwd)) != NULL){
        int i = 0;
        int slash_counter =0;

        while(cwd[i] != '\0'){
            if(cwd[i] == '/'){
                slash_counter++;
            }
            i++;
        }
        if(slash_counter>1){
            strcat(current_directory,replace_with_tilda(cwd,homedir));
        }else{
            strcat(current_directory,cwd);
        }
        strcat(pwd,cwd);
        setenv("PWD",pwd,1);

    }else{
        perror("sfish builtin error: CURRENT DIRECTORY is NULL");
    }
    return current_directory;
}

char* replace_with_tilda(char* cwd, const char* homedir){
    int curr_dir_length = strlen(cwd)-strlen(homedir);
    char cwd_with_tilda[1024] = {0};
    int counter = 0;
    bool finished_home = false;
    char* final_directory_name = calloc(curr_dir_length+2,sizeof(char));
    cwd_with_tilda[0] = '~';
    for(int i=0; i<strlen(cwd); i++){
        if(i == strlen(homedir)){
            finished_home = true;
        }

        if(finished_home){
            cwd_with_tilda[++counter] = cwd[i];
        }
    }
    strcpy(final_directory_name,cwd_with_tilda);

    return final_directory_name;
}

int check_and_execute_builtin_requests(char* input){

    char* argument = calloc(512, sizeof(char*));
    if((argument=validate_and_fetch_arguments(input))){
        if (strcmp(argument, "help")==0){
            print_help(input);
            return 1;
        }else if(strcmp(argument, "exit")==0){
            exit(3);
        }else if(strcmp(argument, "cd")==0){
            cd();
            return 1;
        }else if(strcmp(argument, "cd .")==0){
            cd_dot();
            return 1;
        }else if(strcmp(argument, "cd ..")==0){
            cd_dot_dot();
            return 1;
        }else if(strcmp(argument, "cd -")==0){
            cd_dash();
            return 1;
        }else if(strcmp(argument, "pwd")==0){
            pwd(input);
            return 1;
        }else if(strcmp(argument, "cd dir")==0){
            cd_dir(input);
            return 1;
        }
        else{
            return 0;
        }
    }
    return 0;
}

char* validate_and_fetch_arguments(char* input){

    char** arguments = calloc(1024,sizeof(char));

    arguments = parse_arguments(input, " ");

    if((strcmp(arguments[0],"help")==0)){
        return "help";
    }else if((strcmp(arguments[0],"exit")==0) && arguments[1] ==NULL){
        return "exit";
    }else if((strcmp(arguments[0],"cd")==0) && arguments[1] ==NULL){
        return "cd";
    }else if((strcmp(arguments[0],"cd")==0) && (strcmp(arguments[1],".")==0)){
        return "cd .";
    }else if((strcmp(arguments[0],"cd")==0) && (strcmp(arguments[1],"..")==0)){
        return "cd ..";
    }else if((strcmp(arguments[0],"cd")==0) && (strcmp(arguments[1],"-")==0)){
        return "cd -";
    }else if((strcmp(arguments[0],"cd")==0) && (arguments[1] != NULL)){
        return "cd dir";
    }else if((strcmp(arguments[0],"pwd")==0) && arguments[1] ==NULL){
        return "pwd";
    }else{
        return 0;
    }
    return *arguments;
}

void cd(){
    const char *homedir = getenv("HOME");
    set_current_directory((char*)homedir);
}

void cd_dot(){
    set_current_directory(getenv("PWD"));
}

void cd_dot_dot(){
    char cwd[1024] ={0};
    getcwd(cwd, sizeof(cwd));
    int count =0;
    while(cwd[count]!='\0'){
        count++;
    }

    while(count!=-1){
        if(cwd[count] == '/'){
            if(count==0){
                cwd[count+1] = '\0';
                break;
            }
            cwd[count] = '\0';
            break;
        }
        count--;
    }
    set_current_directory(cwd);
}

void cd_dash(){
    char* prev_directory = getenv("OLDPWD");
    set_current_directory(prev_directory);
}

void cd_dir(char* input){

    char** arguments = calloc(1024,sizeof(char));
    arguments = parse_arguments(input, " ");
    set_current_directory(arguments[1]);
}

void pwd(char* input){
    printf("%s\n",getenv("PWD"));
}

char* get_current_directory(){
    char cwd[1024] ={0};
    getcwd(cwd, sizeof(cwd));
    char* dir = calloc(256,sizeof(char));
    strcpy(dir,cwd);
    return dir;
}

void set_current_directory(char* directory){
    if(chdir(directory)== -1){
        perror("sfish builtin error");

    }else{
        setenv("OLDPWD", getenv("PWD"), 1);
    }
}

char** parse_arguments(char* input, char* break_point){

    char** arguments = calloc(512,sizeof(char*));
    char* temp_input = calloc(512,sizeof(char*));
    strcpy(temp_input,input);
    char * token;
    int count = 1;
    token = strtok (temp_input,break_point);
    arguments[0] = token;
    
    while (token != NULL){
        token = strtok (NULL, break_point);
        arguments[count]=token;
        count++;
      }
    return arguments;
}

char* get_executable_path(char* input){
    bool found = false;
    char** path_array = calloc(strlen(getenv("PATH")),sizeof(char*));
    int count = 0;
    char** arguments = parse_arguments(input," ");
    path_array = get_path_array();
    struct stat buffer;
    int status = 0;
    char* copy = calloc(512,sizeof(char*));


    if(find_slash(arguments[0])){
        status = stat(arguments[0],&buffer);
        if(!status){
            found = true;
            strcpy(copy,arguments[0]);
        }
    }else{

        while(path_array[count] != 0){
            strcpy(copy,path_array[count]);
            strcat(copy,"/");
            strcat(copy,arguments[0]);
            status = stat(copy,&buffer);
            if(status == 0){
                found = true;
                break;
            }
            count++;
        }
    }
    if(!found){
        return 0;
    }

    return copy;

}

int execute(char* input){

    if(!get_executable_path(strdup(input))){
        printf("sfish: %s: command not found\n",input);
        return 1;
    }

    pid_t  pid;
    int status = 0;
    pid = fork();

    if (pid == 0){

      if(execv(get_executable_path(input),parse_arguments(input, " ")) ==  -1){
            perror("sfish exec error: EXECV FAILED\n");
            _exit(EXIT_FAILURE);
      }

    }else if(pid < 0){
        perror("sfish exec error: FORK FAILED\n");
        exit(EXIT_FAILURE);
    }else{

        if(waitpid(pid,&status,0) == -1){
            printf("sfish exec error: WAITPID RETURNED -1 \n");
            exit(EXIT_FAILURE);
        }

    }

    return 1;
}

char**  get_path_array(){

    char* paths = calloc(strlen(getenv("PATH")),sizeof(char*));
    paths = strdup(getenv("PATH"));
    char** single_path = calloc(strlen(getenv("PATH")),sizeof(char*));
    char * token;
    int count = 1;

    token = strtok (paths,":");
    single_path[0] = token;

    while (token != NULL){
        token = strtok (NULL, ":");
        if(token != NULL){
            single_path[count]=token;
        }
            count++;
      }
      free(paths);
      return single_path;
}

bool find_slash(char* args){
    int i =0;
    while(args[i] != '\0'){
            if(args[i] == '/'){
                return true;
            }
            i++;
        }

    return false;
}

bool find_char(char* args, char* element){
    int i =0;
    while(args[i] != '\0'){
            if(args[i] == *element){
                return true;
            }
            i++;
        }

    return false;
}

int count_carrots(char* input){
    int counter = 0;
    while(*input != '\0'){
        if(*input == '>' || *input == '<' ){
            counter++;
        }
        input++;
    }
    return counter;
}

int redirect(char* input){

    int ret = 0;
    bool great = false;
    bool less = false;
    char* greater_than;
    char* less_than;
    char* pipe;
    int greater_than_length = 0;
    int less_than_length = 0;
    int carrot_counter = 0;

    if((carrot_counter = count_carrots(strdup(input))) > 2){
        fprintf(stderr, "sfish syntax error:%s\n","Invalid Command format");
        return 1;
    }else if(is_duplicate(strdup(input),"<")){
        fprintf(stderr, "sfish syntax error:%s\n","Invalid Command format");
        return 1;
    }else if(is_duplicate(strdup(input),">")){
        fprintf(stderr, "sfish syntax error:%s\n","Invalid Command format");
        return 1;
    }else if(find_char(input,"|") && (find_char(input,">") || find_char(input,"<"))){
        fprintf(stderr, "sfish syntax error:%s\n","Invalid Command format");
        return 1;
    }

    if((greater_than =strstr(input,">")) != NULL){
        great = true;
        greater_than_length = greater_than - input;
        ret = 1;
    }

    if((less_than =strstr(input,"<")) != NULL){
        less =  true;
        less_than_length = less_than - input;
        ret = 1;
    }

    if(great && less){
        if(greater_than_length>less_than_length){
            ret = case_three(input,less_than_length);
        }else{
           ret = case_four(input,less_than_length);
        }
    }else if(great){
        if(strcmp(strtok(strdup(input)," "),"help")==0){
            ret = exec_redirect_help(input,">");
        }else{
            ret = case_one(input, greater_than_length);
        }
    }else if(less){
        ret = case_two(input, less_than_length);
    }else if((pipe =strstr(input,"|")) != NULL){
        exec_pipe(input);
        ret =1;
    }else{
        ret = 0;
    }

    return ret;
}

int find_index(char** args, char* character){

    int i =0;
    while(args[i] != '\0'){
        if(*args[i] == *character){
            break;
        }
        i++;
    }

    return i;

}

int case_one(char* input, int sym_index){
    return exec_redirect(input,">");
}

int case_two(char* input, int sym_index){
    return exec_redirect(input,"<");
}

int case_three(char* input, int sym_index){
    int fd_in = 0;
    int fd_out = 0;
    char* input_copy = strdup(input);
    char* prog_input = calloc(512,sizeof(char*));
    char** arguments = calloc(512,sizeof(char*));
    arguments = parse_arguments(input, " ");
    int file_position = find_index(arguments,">") + 1;
    prog_input = strtok(input_copy,"<");

    fd_out = open(arguments[file_position],O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if(fd_out == -1){
        fprintf(stderr, "sfish exec error:%s %s\n",arguments[file_position], strerror(errno));
        return 1;
    }

    file_position = find_index(arguments,"<") + 1;
    fd_in = open(arguments[file_position],O_RDONLY);

    if(fd_in == -1){
        fprintf(stderr, "sfish exec error:%s %s\n",arguments[file_position], strerror(errno));
        return 1;
    }

    int status = 0;
    pid_t pid = fork();

    if(pid == 0){

        if(dup2(fd_in,0) == -1){
            perror("sfish exec error: Dup2 failed\n");
            _exit(EXIT_FAILURE);
        }
        if(dup2(fd_out,1) == -1){
            perror("sfish exec error: Dup2 failed\n");
            _exit(EXIT_FAILURE);
        }
        if(fd_out != STDOUT_FILENO || fd_in != STDIN_FILENO){
            close(fd_out);
            close(fd_in);
        }
       if(execv(get_executable_path(prog_input),parse_arguments(prog_input, " "))){
            perror("sfish exec error: execv failed\n");
            _exit(EXIT_FAILURE);
        }

    }
    else if(pid < 0){
        perror("sfish exec error: Forking Failed\n");
        _exit(EXIT_FAILURE);
    }
    else{

        if(waitpid(pid,&status,0) == -1){
            printf("sfish exec error: WAITPID RETURNED -1 \n");
            exit(EXIT_FAILURE);
        }

    }
    return 1;
}

int case_four(char* input, int sym_index){
    int fd_in = 0;
    int fd_out = 0;
    char* input_copy = strdup(input);
    char* prog_input = calloc(512,sizeof(char*));
    char** arguments = calloc(512,sizeof(char*));
    arguments = parse_arguments(input, " ");
    int file_position = find_index(arguments,">") + 1;
    prog_input = strtok(input_copy,">");

    fd_out = open(arguments[file_position],O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if(fd_out == -1){
        fprintf(stderr, "sfish exec error:%s %s\n",arguments[file_position], strerror(errno));
        return 1;
    }
    
    file_position = find_index(arguments,"<") + 1;
    fd_in = open(arguments[file_position],O_RDONLY);
    if(fd_in == -1){
        fprintf(stderr, "sfish exec error:%s %s\n",arguments[file_position], strerror(errno));
        return 1;
    }
    
    int status = 0;
    pid_t pid = fork();
    
    if(pid == 0)
    {           /* child */

        if(dup2(fd_in,0) == -1){
            perror("sfish exec error: Dup2 failed\n");
            _exit(EXIT_FAILURE);
        }
        if(dup2(fd_out,1) == -1){
            perror("sfish exec error: Dup2 failed\n");
            _exit(EXIT_FAILURE);
        }
        if(fd_out != STDOUT_FILENO || fd_in != STDIN_FILENO){
            close(fd_out);
            close(fd_in);
        }
        if(execv(get_executable_path(prog_input),parse_arguments(prog_input, " "))){
            perror("sfish exec error: execv failed\n");
            _exit(EXIT_FAILURE);
        }

    }
    else if(pid < 0){           /* failed to fork */
        perror("sfish exec error: Fork failed\n");
        _exit(EXIT_FAILURE);
    }
    else{                       /* parent */
        if(waitpid(pid,&status,0) == -1){
            perror("sfish exec error: wait pid is -1\n");
            _exit(EXIT_FAILURE);
        }

    }
    return 1;
}

int exec_redirect(char* input,char* direction){
    int fd = 0;
    int FILE_NO = 0;
    char* prog_input = calloc(512,sizeof(char*));
    char** arguments = calloc(512,sizeof(char*));
    arguments = parse_arguments(input, " ");
    int file_position = find_index(arguments,direction) + 1;
    prog_input = strtok(input,direction);

    if(*direction == '>'){
        FILE_NO = STDOUT_FILENO;
        fd = open(arguments[file_position],O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        if(fd == -1){
            fprintf(stderr, "sfish exec error:%s %s\n",arguments[file_position], strerror(errno));
            return 1;
    }
    }else{
        FILE_NO = STDIN_FILENO;
        fd = open(arguments[file_position],O_RDONLY);
        if(fd == -1){
        fprintf(stderr, "sfish exec error:%s %s\n",arguments[file_position], strerror(errno));
        return 1;
    }
    }

    if(!get_executable_path(strdup(prog_input))){
        printf("sfish: %s: command not found\n",prog_input);
        return 1;
    }
    int status = 0;
    pid_t pid = fork();

    if(pid == 0){           /* child */
        if(dup2(fd,FILE_NO) == -1){
            return 0;
        }
        if(fd != FILE_NO){
            close(fd);
        }
        if(execv(get_executable_path(prog_input),parse_arguments(prog_input, " "))< 0){
            _exit(3);
            perror(EXEC_ERROR);
        }
    }else if(pid < 0){           /* failed to fork */
        printf("Failed To Fork\n");
    }
    else{

        if(waitpid(pid,&status,0) == -1){
            perror(EXEC_ERROR);
        }
    }
    return 1;
}

int validate_pipes(char** arguments){
    int counter = 0;
    int status = 0;
    struct stat buffer;
    char* token;
    while(arguments[counter] != 0){
        token = strtok(strdup(arguments[counter]), " ");
        if(stat(token,&buffer)){
            status = counter;
            break;
        }
        counter++;
    }
        return status+1;
}

int exec_pipe(char* input){
    char* prog_input = calloc(512,sizeof(char*));
    char** arguments = calloc(512,sizeof(char*));
    arguments = parse_arguments(input, "|");
    int total_pipes = array_size(arguments);
    int pipefds[2*total_pipes];
    int status;
    int i = 0;
    pid_t pid;
    
    /* parent creates all needed pipes at the start */
    for( int i = 0; i < total_pipes; i++ ){
        if( pipe(pipefds + i*2) < 0 ){
            perror("sfish exec error: Pipe failed\n");
            exit(EXIT_FAILURE);
        }
    }
    int j = 0;
    int counter = 0;
    while(arguments[counter] != 0) {
        if(!get_executable_path(strdup(arguments[counter]))){
            printf("sfish: %s: command not found\n",arguments[counter]);
            counter++;
            j += 2;
            if(arguments[counter] == 0){
                return 1;
            }
            continue;
        }
        pid = fork();
        if(pid == 0) {
            //if not last command
            if(arguments[counter+1]){
                if(dup2(pipefds[j + 1], 1) < 0){
                    perror("sfish exec error: Dup2 failed\n");
                    _exit(EXIT_FAILURE);
                }
            }
            
            if(j != 0 ){
                if(dup2(pipefds[j-2], 0) < 0){
                    perror("sfish exec error: Dup2 failed\n");
                    _exit(EXIT_FAILURE);

                }
            }
            
            for(i = 0; i < 2*total_pipes; i++){
                    close(pipefds[i]);
            }

            prog_input = strtok(strdup(arguments[counter])," ");

            if( execv(get_executable_path(prog_input), parse_arguments(arguments[counter], " ")) == -1 ){
                    perror("sfish exec error: Cannot execute program\n");
                    _exit(EXIT_FAILURE);
            }
        } else if(pid < 0){
            perror("sfish exec error: Fork failed\n");
            _exit(EXIT_FAILURE);
        }
        counter++;
        j+=2;

    }
    /**Parent closes the pipes and wait for children*/
    for(i = 0; i < 2 * total_pipes; i++){
        close(pipefds[i]);
    }

    for(i = 0; i < total_pipes + 1; i++){
        wait(&status);
    }
    return 1;
}

int array_size(char** array){
    int counter = 0;
    while(array[counter] != 0){
        counter++;
    }
    return counter;
}

int exec_redirect_help(char* input,char* direction){

    int fd = 0;
    int FILE_NO = 0;
    char** arguments = calloc(512,sizeof(char*));
    arguments = parse_arguments(input, " ");
    int file_position = find_index(arguments,direction) + 1;
    
    if(*direction == '>'){
        FILE_NO = STDOUT_FILENO;
        fd = open(arguments[file_position],O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        if(fd == -1){
        fprintf(stderr, "sfish exec error:%s %s\n",arguments[file_position], strerror(errno));
        return 1;
    }
    }else{
        FILE_NO = STDIN_FILENO;
        fd = open(arguments[file_position],O_RDONLY);
        if(fd == -1){
        fprintf(stderr, "sfish exec error:%s %s\n",arguments[file_position], strerror(errno));
        return 1;
    }
    }
    int status = 0;
    pid_t pid = fork();

    if(pid == 0)
    {           /* child */
        if(dup2(fd,FILE_NO) == -1){
            perror("sfish exec error: Dup2 failed\n");
            _exit(EXIT_FAILURE);
        }
        if(fd != FILE_NO){
            close(fd);
        }
        printf("%s","Shell written by Ali Hamdany\n");
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        printf("help: Prints a list of all builtin functions and their basic usage.\nexit: Exits the shell by using the exit(3) function.\ncd: Changes the current working directory of the shell.\n   cd - should change the working directory to the last directory the user was in.\n   cd with no arguments should go to the user's home directory which is stored in the HOME environment variable.\n   cd . and cd .. should be handled correctly. . and .. are special directories that appear in every directory of a Unix system. They correspond to the current directory and previous directory of the current working directory.\npwd: Prints the absolute path of the current working directory. This can be obtained by using the getcwd(3) function.\n");
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        exit(3);
    }
    else if(pid < 0)
    {
        perror("sfish exec error: Failed to fork\n");
        _exit(EXIT_FAILURE);
    }
    else
    {                   /* parent */
        if(waitpid(pid,&status,0) == -1){
            perror("sfish exec error: wait pid is -1\n");
            _exit(EXIT_FAILURE);
        }
    }

    return 1;
}

























