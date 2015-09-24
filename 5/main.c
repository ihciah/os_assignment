/*This is a simple implement of shell
 * @ihciah
 * 13307130364@fudan.edu.cn
 *
 * Features:
 * history
 * run in background
 * cd
 * pwd
 * !!  !n
 * exit
 * auto replace ~ with home dir path
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define CMD_MAX_LENGTH 1024
#define MAX_ARGC (CMD_MAX_LENGTH / 2 + 1)
#define PATH_MAX_LENGTH 1024
int his_count;
char* cmdx[MAX_ARGC];
char* cmdx_mod[MAX_ARGC];
int cmd_argc;

struct History{
    struct History* next;
    int n;
    char cmd[CMD_MAX_LENGTH];
};
struct History *his_head;

//Replace the first `~` to HOME path
void ReplaceFirstC(char* str){
    if(str[0]=='~'){
        char tmparg[CMD_MAX_LENGTH];
        char *hd=getenv("HOME");
        if(hd!=NULL){
            strncpy(tmparg,hd,CMD_MAX_LENGTH);
            strncat(tmparg,str+1,CMD_MAX_LENGTH-strlen(hd)-10);
            strncpy(str,tmparg,CMD_MAX_LENGTH);
        }
    }
}

//Get the Nth command in history
bool GetNthHistory(int n,char* s){
    if(n<1||n>his_count)
        return false;
    struct History *p=his_head;
    while(p){
        if(p->n==n){
            strncpy(s,p->cmd,CMD_MAX_LENGTH);
            return true;
        }
        p=p->next;
    }
    return false;
}

//Add a command to history
void PushHistory(const char* s){
    his_count+=1;
    struct History *h=(struct History*)malloc(sizeof(struct History));
    strncpy(h->cmd,s,CMD_MAX_LENGTH);
    h->n=his_count;
    h->next=his_head;
    his_head=h;
}

//Print all history
void PrintHistory(){
    struct History *p=his_head;
    while(p){
        printf("%d %s",p->n, p->cmd);
        p=p->next;
    }
}

//Split the command
bool SplitCommand(const char* s,int* argc){
    memcpy(cmdx_mod,cmdx,MAX_ARGC*sizeof(char*));
    char cmd[CMD_MAX_LENGTH];
    char seps[]=" ,\t\n";
    char* token;
    *argc=0;
    strncpy(cmd,s,CMD_MAX_LENGTH);
    token=strtok(cmd,seps);
    while(token!=NULL){
        strncpy(cmdx_mod[(*argc)],token,CMD_MAX_LENGTH);
        ReplaceFirstC(cmdx_mod[(*argc)++]);
        token=strtok(NULL,seps);
    }
    cmdx_mod[*argc]=NULL;
    return true;
}

//Print current path
void Printpwd(){
    char current_path[PATH_MAX_LENGTH];
    getcwd(current_path, sizeof(current_path));
    printf("%s",current_path);
}

//Execute other programs
void real_exec(){
    pid_t pid=fork();
    bool background=false;
    if(cmd_argc>=2&&strncmp(cmdx_mod[cmd_argc-1],"&",CMD_MAX_LENGTH)==0){
        cmdx_mod[cmd_argc-1]=NULL;
        background=true;
    }
    if(pid<0){
        return;
    }
    else if(pid==0){
        if(execvp(cmdx_mod[0],cmdx_mod)==-1){
            printf("exec error.\n");
            exit(0);
        }
    }
    if(!background){
        waitpid(pid,NULL,0);
    }
}

//Execute commands
void exec(char* cmd,bool* should_run){
    SplitCommand(cmd,&cmd_argc);
    if(cmd_argc==0||strncmp(cmdx_mod[0],"&",CMD_MAX_LENGTH)==0){
        printf("Invalid command.\n");
        return;
    }
    if(strncmp(cmdx_mod[0],"history",CMD_MAX_LENGTH)==0){
        PrintHistory();
        PushHistory(cmd);
        return;
    }
    if(strncmp(cmdx_mod[0],"exit",CMD_MAX_LENGTH)==0){
        printf("Bye\n");
        PushHistory(cmd);
        exit(0);
        return;
    }
    if(strncmp(cmdx_mod[0],"!!",CMD_MAX_LENGTH)==0){
        char cmd_last[CMD_MAX_LENGTH];
        bool res=GetNthHistory(1,cmd_last);
        if(res){
            SplitCommand(cmd_last,&cmd_argc);
            //PushHistory(cmd_last);
            exec(cmd_last,should_run);
        }
        else{
            printf("Invalid command.\n");
        }
        return;
    }
    if(cmdx_mod[0][0]=='!'){
        char cmd_last[CMD_MAX_LENGTH];
        int nlast=-1;
        sscanf(cmdx[0]+1,"%d",&nlast);
        bool res=GetNthHistory(nlast,cmd_last);
        if(res&&nlast!=-1){
            SplitCommand(cmd_last,&cmd_argc);
            //PushHistory(cmd_last);
            exec(cmd_last,should_run);
        }
        else{
            printf("Invalid command.\n");
        }
        return;
    }
    if(strncmp(cmdx_mod[0],"cd",CMD_MAX_LENGTH)==0){
        if(!cmdx_mod[1]){
            printf("Invalid path.\n");
            PushHistory(cmd);
            return;
        }
        int checkDir = chdir(cmdx_mod[1]);
        if (checkDir == -1)
        {
            printf("Invalid path.\n");
        }
        PushHistory(cmd);
        return;
    }
    if(strncmp(cmdx_mod[0],"pwd",CMD_MAX_LENGTH)==0){
        char currpath[PATH_MAX_LENGTH];
        getcwd(currpath, PATH_MAX_LENGTH);
        PushHistory(cmd);
        printf("%s\n",currpath);
        return;
    }
    real_exec();
    PushHistory(cmd);
}


int main(void){
    int ii;
    for(ii=0;ii<MAX_ARGC;ii++){
        cmdx[ii]=(char*)malloc(sizeof(char)*CMD_MAX_LENGTH);
    }
    bool should_run=true;
    his_count=0;
    his_head=NULL;
    char cmd[CMD_MAX_LENGTH];
    while(should_run){
        write(STDOUT_FILENO,"osh> ",sizeof("osh> "));
        fgets(cmd,CMD_MAX_LENGTH,stdin);
        exec(cmd,&should_run);
    }
    return 0;
}
