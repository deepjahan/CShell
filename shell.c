#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <fcntl.h>

char HOME_DIR[PATH_MAX];//stores the home directory
int HOME_DIR_LEN;

typedef struct proc {
	int pid;
	int job;
	char name[100];
}proc;

//Displays the basic terminal(username,system name and current directory)
int dispCurrDir()
{
	char hostname[HOST_NAME_MAX];
	int host_ret=gethostname(hostname,sizeof(hostname));
	char *user = getenv("USER");
	char cwd[PATH_MAX];
	getcwd(cwd,sizeof(cwd));
	if(user && !host_ret)
	{
		if(!strncmp(HOME_DIR,cwd,HOME_DIR_LEN))
		{
			if(cwd[HOME_DIR_LEN]=='\0')
				printf("<%s@%s:~>",user,hostname);
			else
				printf("<%s@%s:~%s>",user,hostname,cwd+HOME_DIR_LEN);
		}
		else
		{
			printf("<%s@%s:%s>",user,hostname,cwd);
		}
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

char *strstrip(char *s)
{
        size_t size;
        char *end;

        size = strlen(s);

        if (!size)
                return s;

        end = s + size - 1;
        while (end >= s && isspace(*end))
                end--;
        *(end + 1) = '\0';

        while (*s && isspace(*s))
                s++;

        return s;
}

static int *exit_status; //If someone types exit or quit, this variable will get value 1
static int *exit_msg;
static int *parent_id; //stores the pid of the shell program
proc plist[100];
int pptr=1;

void  INThandler(int sig)
{
     //char  c;

	signal(sig, SIG_IGN);
	int pid=getpid();
	kill(pid,SIGTSTP);
	plist[pptr].job=pptr;
	plist[pptr].pid=pid;
	strcpy(plist[pptr++].name,"Yay");
	exit(0);
     //printf("OUCH, did you hit Ctrl-C?\n"
     //       "Do you really want to quit? [y/n] ");
     //c = getchar();
     //if (c == 'y' || c == 'Y')
     //     exit(0);
     //else
     //     signal(SIGINT, INThandler);
     //getchar(); // Get new line character
}

void handler(int sig) {
}

int main()
{
	int bg_pid[1000];
	int bg_pid_ptr=0;
	int catch_int;
	char dumm;
	struct sigaction act;
	act.sa_handler=handler;
	sigaction(SIGCHLD,&act,NULL);
	//signal(SIGINT, SIG_IGN);
	//signal(SIGTSTP, SIG_IGN);
	signal(SIGTSTP, INThandler);
	int pid=0,pid2=0;
	int status,status2,status3;
	char error[100];
	getcwd(HOME_DIR,sizeof(HOME_DIR));
	HOME_DIR_LEN=strlen(HOME_DIR);
	char cmmnd[sysconf(_SC_ARG_MAX)];
	char *ch;
	int bg=0;
	char cwd[PATH_MAX];
	//Making few global variables accessible to all threads
	exit_status = mmap(NULL, sizeof *exit_status, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*exit_status=0;
	exit_msg = mmap(NULL, sizeof *exit_msg, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*exit_msg=0;
	parent_id = mmap(NULL, sizeof *parent_id, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*parent_id=getpid();
	while(1) {
		bg=0;
		//pid=fork();
		if(pid==0) //child process
		{
			pid2=0;
			dispCurrDir();
			strcpy(cmmnd,"");
			int scin=scanf("%[^\n]",cmmnd);
			scanf("%c",&dumm);
			if(!strcmp(cmmnd,""))
			{
				continue;
			}
			/*int iter=0;
			while(cmmnd[iter]!=0)
			{
				if(cmmnd[iter]==26)
				{
					int curr_pid=getpid();
					kill(curr_pid,SIGTSTP);
					pl
				}
				iter++;
			}*/
			char *cmmnd1=strtok(cmmnd,";"); //Separating semicolon seperated commands
			//printf("%c\n",cmmnd1[0] );
			/*if(cmmnd1[0]=='d')
				*exit_status=1;*/
			//break;
			int stin,stout;
			stin=dup(0);
			stout=dup(1);
			while(cmmnd1)
			{
				
					char *redl1=strtok(cmmnd1,"<");
					char *redl2=strtok(NULL,"<");
					char *redg1=strtok(redl1,">");
					char *redg2=strtok(NULL,">");
					char *redg3=strtok(redl2,">");
					char *redg4=strtok(NULL,">");
					
					//printf("%s %s %s %s\n",redg1,redg2,redg3,redg4);
					int fg24,fg3;
					if(redg2!=NULL)
					{
						redg2=strstrip(redg2);
						fg24=open(redg2, O_WRONLY | O_CREAT , S_IRWXU);
						dup2(fg24,1);
						close(fg24);
					}
					else if(redg4!=NULL)
					{
						redg4=strstrip(redg4);
						fg24=open(redg4, O_WRONLY | O_CREAT , S_IRWXU);
						dup2(fg24,1);
						close(fg24);
					}
					if(redg3!=NULL)
					{
						redg3=strstrip(redg3);
						fg3=open(redg3, O_RDONLY | O_CREAT , S_IRWXU);
						dup2(fg3,0);
						close(fg3);
					}
					char *pipes[100];
					char *ch;
					ch=strtok(redg1,"|");
					int p_i=0;
					while(ch)
					{
						pipes[p_i]=ch;
						ch=strtok(NULL,"|");
						p_i+=1;
					}
					int in, fd [2],out;
					//printf("%s - %s\n", pipes[0], pipes[1]);
					in=0;
					int jj;
					if(p_i==1)
						in=-11;
					for(jj=0;jj<p_i;++jj)
					{
						pipe(fd);
						out=fd[1];
						//spawn_proc (in, fd [1], pipes + j);
						
						char *name=strtok(pipes[jj]," ");
						char *args[100];
						ch=strtok(NULL," ");
						args[0]=name;
						int i=1;
						while(ch) //Breaking the command into arguments to be accessed by execvp
						{
							args[i]=ch;
							ch=strtok(NULL," ");
							i+=1;
						}
						args[i]=NULL;
						//printf("%s\n", args[0]);
						bg=0;
						if(!strcmp(args[i-1],"&"))
						{
							bg=1;
							args[i-1]=NULL;

						}
						int pid3=fork();
						if(pid3>0)
						{

							if(bg==1)
							{
								plist[pptr].job=pptr;
								plist[pptr].pid=pid3;
								strcpy(plist[pptr].name,pipes[jj]);
								printf("[%d] %s [%d] running in background\n",plist[pptr].job,plist[pptr].name,plist[pptr].pid);
								pptr++;
								pid3=waitpid(-pid3,&status3,WNOHANG);
							}
							else
								pid3=waitpid(pid3,&status3,0);
						}
						else if(pid3==0)
						{

							if(in!=-11)
							{
								if(jj!=p_i-1)
								{
									if(in!=0)
									{
										dup2 (in, 0);
			          					close (in);
									}
									if(out!=1)
									{
										dup2 (out, 1);
			          					close (out);
									}
								}
								else
								{
									if (in != 0)
		    							dup2 (in, 0);
								}
							}
							if(!strcmp(name,"exit") || !strcmp(name,"quit"))
							{
								*exit_status=1;
								exit(0);
							}
							
							int stat;
							/*if(bg==1)
							{
								pid2=fork(); //Running command in background
							}
							if(pid2>0)
							{
								pid2=waitpid(-pid2,&status2,WNOHANG);
								//printf("%d %d\n",pid2,status2 );
								//exit(0); //Making the command running in background a daemon process
							}*/
							if(pid2==0)
							{
								if(bg==1)
								{
									
									setpgid(0, 0);
									//setvbuf(stdin,NULL,_IONBF, 0);
									//setvbuf(stdout,NULL,_IONBF, 0);
									//fflush(stdin);
									//fflush(stdout);
								}

								if(!strcmp("jobs",name))
								{
									int j;
									for(j=1;j<pptr;j++)
									{
										char fname[50];
										char chw[50];
										FILE* fp;
										strcpy(fname,"/proc/");
										char buffer[10];
										sprintf(buffer, "%d", plist[j].pid);
										strcat(fname,buffer);
										strcat(fname,"/stat");
										fp=fopen(fname,"r");
										int j_j;
										char Statu[3];
										if(fp)
										{
											for(j_j=1;j_j<=3;++j_j)
											{
												fscanf(fp,"%s",chw);
												if(j_j==3)
													strcpy(Statu,chw);
											}
										}
											
										fclose(fp);
										if(strcmp(Statu,"Z") && fp)
										{
											printf("[%d] %s [%d]\n",plist[j].job,plist[j].name,plist[j].pid);
										}
									} 
									exit(0);
								}
								if(!strcmp("kjob",name))
								{
									//printf("%d\n",plist[atoi(args[1])].pid);
									kill(plist[atoi(args[1])].pid,atoi(args[2]));
									exit(0);
								}
								if(!strcmp("fg",name))
								{
									setpgid(plist[atoi(args[1])].pid,getpgid(getpid()));
									kill(getpid(),SIGKILL);
									kill(plist[atoi(args[1])].pid,SIGCONT);
									exit(0);
								}
								if(!strcmp("overkill",name))
								{
									int j;
									for(j=1;j<pptr;j++)
									{
										kill(plist[j].pid,SIGKILL);
									} 
									exit(0);
								}
								if(execvp(name,args)<0)
								{
									if(!strcmp("cd",name)) //implementing cd
									{
										if(args[1])
										{
											if(args[1][0]=='/')
											{
												stat=chdir(args[1]);
											//	strcpy(cwd,args[1]);
											}
											else if(args[1][0]=='~')
											{
												strcpy(cwd,HOME_DIR);
												if(args[1][1]=='/')
													strcat(cwd,args[1]+1);
												else
													strcat(cwd,args[1]+1);
												stat=chdir(cwd);
											}
											else
											{
												getcwd(cwd,sizeof(cwd));
												strcat(cwd,"/");
												strcat(cwd,args[1]);
												stat=chdir(cwd);
											}
										}
										else
										{
											printf("%sf\n",HOME_DIR);
											stat=chdir(HOME_DIR);
											//strcpy(cwd,HOME_DIR);
										}
										if(stat<0)
										{
											strcpy(error,args[1]);
											strcat(error," cannot be found");
											perror(error);
											exit(-1);
										}
										
										if(bg==1)
											exit(0);
									}
									else if(!strcmp("pinfo",name)) //implementing pinfo
									{
										if(args[1])
										{
											char fname[50];
											char chw[50];
											FILE* fp;
											strcpy(fname,"/proc/");
											strcat(fname,args[1]);
											strcat(fname,"/stat");
											fp=fopen(fname,"r");
											int j;
											char Statu[3];
											char vsize[15];
											if(fp)
											{
												for(j=1;j<=23;++j)
												{
													fscanf(fp,"%s",chw);
													if(j==3)
														strcpy(Statu,chw);
													else if(j==23)
														strcpy(vsize,chw);
												}
											
												fclose(fp);
												strcpy(fname,"/proc/");
												strcat(fname,args[1]);
												strcat(fname,"/cmdline");
												fp=fopen(fname,"r");
												if(fp)
												{
													char cmdline[100];
													fscanf(fp,"%s",cmdline);
													printf("pid: %s\nStatus: %s\nMemory: %s\nExecutable Path: %s\n",args[1],Statu,vsize,cmdline);
												}
												else
													printf("Invalid Process ID: %s\n",args[1]);
											}
											else
											{
												printf("Invalid Process ID: %s\n",args[1]);
											}
										}
										else
										{
											char fname[50];
											FILE* fp;
											fp=fopen(fname,"r");
											char chw[50];
											int j;
											sprintf(chw,"%d",*parent_id);
											char ID[15];
											char Statu[3];
											char vsize[15];
											strcpy(ID,chw);
											strcpy(fname,"/proc/");
											strcat(fname,ID);
											strcat(fname,"/stat");
											fp=fopen(fname,"r");
											for(j=1;j<=23;++j)
											{
												fscanf(fp,"%s",chw);
												if(j==3)
													strcpy(Statu,chw);
												else if(j==23)
													strcpy(vsize,chw);
											}
											fclose(fp);
											strcpy(fname,"/proc/");
											strcat(fname,ID);
											strcat(fname,"/cmdline");
											fp=fopen(fname,"r");
											char cmdline[100];
											fscanf(fp,"%s",cmdline);
											printf("pid: %s\nStatus: %s\nMemory: %s\nExecutable Path: %s\n",ID,Statu,vsize,cmdline);
										}
									}
									else
									{
										strcpy(error,name);
										strcat(error," command cannot be run");
										perror(error);
										exit(-1);
									}
								}
							
							}
						}
					if(in!=-11)
					{
						close(out);
						in=fd[0];
					}
					else
					{
						dup2(stout,1);
						close(stout);
						dup2(stin,0);
						close(stin);
					}
					
				}
				
					
				//else
				//	pid3=waitpid(-pid3,&status,WNOHANG);
				if(*exit_status==1)
					exit(0);
				cmmnd1=strtok(NULL,";");
			}
		}
		/*if(bg!=1)
			pid=waitpid(pid,&status,0);
		else
			pid=waitpid(-pid,&status,WNOHANG);
		*/if(*exit_status==1)
		{
			if(*exit_msg==0)
			{
				*exit_msg=1;
				printf("\nExiting the shell normally!!!\n\n");
			}
			exit(0);
		}
	}
	return 0;
}
