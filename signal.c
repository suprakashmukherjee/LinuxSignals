//************************************************************
//  Suprakash Mukherjee (2019H1120048P)
//  ME SS, BITS Pilani
//  NP Lab Assignment 1 (odd semester 2020)
//
//  Logic Explanation : 
//  N and M are taken input from the user.
//  Parent process creates N child processes. When a process with even pid is created, the pid is added to the evn_process[].
//  Also the count of even processes is stored in evn_process_count
//  
//  The even processes are waiting for signal by using pause() function
//  Odd processes are continuously sending SIGUSR1 signal to even processes before it.
//  If there is no even process created before an odd process then it waits for signal using pause()
//
//************************************************************


#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int Mcount = 0;     //keeps count of number of SIGUSR1 received by an even process id
int M;              //indicates threshold


void sig_handler(int signum, siginfo_t *siginfo, void *context)
{
    if(signum == SIGUSR1)
        {
            
            Mcount++;
            pid_t pid_sender = siginfo->si_pid;
            printf("Process pid=%d | SIGUSR1 sent by pid=%d | Signals received count=%d\n",getpid(),pid_sender,Mcount);

            if(Mcount>M)
            {
                //terminate sender and receiver
                
                kill(pid_sender,SIGTERM);
                //printf("Sending Termination to odd sender process=%d by even process=%d\n",pid_sender,getpid());
                sleep(1);   //waiting for odd process to handle SIGTERM before sending SIGKILL
                kill(pid_sender,SIGKILL);
                
                // Terminate self
                printf("%d Terminated Self | Last SIGUSR1 sender pid=%d | Signals received count=%d\n",getpid(),pid_sender,Mcount);
                exit(0);
            }
                    
        }

    else if(signum == SIGTERM)
        {
            pid_t pid_sender = siginfo->si_pid;

            printf("%d Terminated by %d\n",getpid(),pid_sender);
            exit(0);

        }
}




int main ()
{
    int N;
    
    pid_t evn_process[100];         //contains even pids created by parent
    int evn_process_count = 0;      //count of pids in the evn_process
    pid_t pid;  

    printf("Enter the value of N and M: separated by space:\n");
    scanf("%d %d",&N,&M);

    printf("\nParent process id = %d\n\n", getpid());







    //************fork*****************

    for(int i=0;i<N;i++)
    {

        //********ERROR Creating process*************
        if ((pid = fork ()) < 0)
        {
            perror ("fork"); //function to print error that occurred in the process
        }



        //************child process code*****************
        else if (pid == 0)
        {        
            pid = getpid();
            printf ("Child process created with pid = %d\n", pid);       // getpid() gets the process id of current




            if(pid%2==0)        //even pid receive SIGUSR1
            {
                
                struct sigaction sa;
                sa.sa_handler = &sig_handler;
                sa.sa_flags = SA_SIGINFO;

                sigaction(SIGUSR1, &sa, NULL);
                sigaction(SIGTERM, &sa, NULL);

                //signal(SIGUSR1,sig_handler); // Register signal handler
                
                
                while(1)
                {
                    pause();   //waiting for signal to interrupt even process
                }
            }





            else                //odd pid sends SIGUSR1
            {
                struct sigaction sa;
                sa.sa_handler = &sig_handler;
                sa.sa_flags = SA_SIGINFO;

                sigaction(SIGTERM, &sa, NULL);

                //signal(SIGTERM,sig_handler);
                
                
                while(1)
                {
                    if(evn_process_count>0)
                    {
                        int pid_target;
                        while(1)
                        {
                            int randomNum =(rand() % (evn_process_count - 1 + 1)) + 1;      //random number between 1 to evn_process_count generated
                            pid_target = evn_process[randomNum];        //picking up pid of even process from the array using the randomly generated number
                            kill(pid_target,SIGUSR1);
                        }                        
                    }
                    
                    else
                    {
                        //printf("No even process spawned before current process with pid=%d",pid);
                        pause();
                    }
                    
                    
                }
                
                             
            }
            

        }
        
        else        //executed by parent
        {
            //printf ("printing from parent - child process created with pid = %d by parent id = %d\n", pid, getpid());
            
            if(pid%2==0)
            {
                evn_process_count++;
                evn_process[evn_process_count] = pid;   //storing pids of even processes from array location 1 onwards
                sleep(1);   //added because sometimes the even process is getting killed as soon as it is created.
                            //in such cases the the process is getting killed without the signal being caught by the handler.
            }
            
        }
    }


    printf("Terminating Parent %d\n", getpid());
    exit (0);


}

