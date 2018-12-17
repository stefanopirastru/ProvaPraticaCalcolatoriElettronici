//
// Created by Stefano Pirastru on 05/11/18.
//
#include <stdio.h> //for printf
#include <stdlib.h> //for exit
#include <getopt.h> //for getopt_long
#include <string.h> //for strcmp
#include <time.h> //clock()
#include <sys/types.h>
#include <sys/wait.h> //wait()
#include <unistd.h>//fork()

#include "main.h" //run()


int main(int argc, char **argv){
    int c,count;
    count = 0;
    int digit_optind = 0;
    int semaphore = 0;
    extern int opterr;
    char *argumentsp=NULL;
    char *arguments=NULL;
    char *dot=NULL;
    if(argc!=7){
        printf("helper\n");
        printf("compile with:\n");
        printf("./simulator -op output_pre.csv -on output_not_preemptive.csv -i tasks.csv\n");
        exit(0);
    }else{
        if(((strcmp(argv[1],"-op")==0)||(strcmp(argv[1],"--output-preemption")==0))&&((strcmp(argv[3],"-on")==0)||(strcmp(argv[3],"--output-no-preemption")==0))&&((strcmp(argv[5],"-i")==0)||(strcmp(argv[5],"--input")==0))){

        }else{
            printf("helper\n");
            printf("compile with:\n");
            printf("./simulator -op output_pre.csv -on output_not_preemptive.csv -i tasks.csv\n");
            exit(0);
        }
    }

    opterr=0;
    //printf("argc %d\n",argc);
    //for(int i = 0;i<argc;i++) printf("i:%d value: %s\n", i, argv[i]);
    while(count<=5){
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
                {"help",                  no_argument,       NULL, 'h'},
                {"output-preemption",     required_argument, NULL, 'o'},
                {"output-no-preemption",  required_argument, NULL, 'p'},
                {"input",                 required_argument, NULL, 'i'}
        };

        c = getopt_long(argc, argv, "abc:d:012", long_options, &option_index);
        if(count == 0) {
            printf("%d\n",option_index);
            count++;
        }
        if(semaphore == 1){
            semaphore = 0;
            printf("count %d\n", count);
            count++;
        }
        if(strcmp(argv[count], "-h")  == 0) c = 'h';

        if((strcmp(argv[count], "-op") == 0 )&& (count ==1)){
            c = 'o';
            argumentsp = argv[count+1];
            semaphore = 1;
        }

        if((strcmp(argv[count], "-on") == 0 )&& (count ==3)){
            c = 'p';
            arguments = argv[count+1];
            semaphore = 1;
        }

        if((strcmp(argv[count], "-i") == 0)&& (count ==5)){
            c = 'i';
            dot = argv[count+1];
            semaphore = 1;
            printf("really? %s\n", argv[count+1]);
        }

        //printf("%d\n",c);
        switch(c){
            case 0:
                printf("option %s",long_options[option_index].name);
                if(optarg) printf(" with argum %s\n", optarg);
                if(arguments) printf(" with arg %s\n", arguments);
                printf("\n");
                break;
            case 'o':
                printf("preemption\n");
                if(optarg){
                    printf(" with argum %s\n", optarg);
                    argumentsp = optarg;
                }
                if(argumentsp) printf(" with arg %s\n", argumentsp);
                break;
            case 'p':
                printf("no preemption\n");
                if(optarg){
                    printf(" with argum %s\n", optarg);
                    arguments = optarg;
                }
                if(arguments) printf(" with arg %s\n", arguments);
                break;
            case 'i':
                printf("input \n");
                if(count == 5 ) dot = argv[count+1];
                if(optarg && count!=6 && count==5){
                    printf(" with argum %s\n", optarg);
                    dot = optarg;
                    count = 6;
                }
                printf("in i  count %d\n",count);
                if(dot) printf(" with arg %s\n", arguments);
                break;
            case 'h':
                printf("helper\n");
                printf("run with:\n");
                printf("./simulator --output-preemption output_pre.csv --output-no-preemption output_not_preemptive.csv --input tasks.csv\n");
                printf("or equally use\n");
                printf("./simulator -op output_pre.csv -on output_not_preemptive.csv -i tasks.csv\n");
                break;
            case '?' :
                break;
            default:
                printf("helper\n");
                printf("run with:\n");
                printf("./simulator --output-preemption output_pre.csv --output-no-preemption output_not_preemptive.csv --input tasks.csv\n");
                printf("or equally use\n");
                printf("./simulator -op output_pre.csv -on output_not_preemptive.csv -i tasks.csv\n");
                break;

        }count++;
    }
    if(optind < argc){
        printf("non-option ARGV-elements: ");
        while(optind < argc)
            printf("%s ",argv[optind++]);
        printf("\n");
    }

    //stage 2 routing into processes

    pid_t pid;
    int status = -2;
    int check_pre = -1;
    int check_no_pre = -1;
    clock_t start_1, start_2, end_1, end_2;
    start_1 = start_2 = end_1 = end_2 = 0;
    printf("Father pid: (%d)\n", getpid());
    for (int i = 0; i < 2; i++) {
        if ((pid = fork()) == -1)
            exit(EXIT_FAILURE); // error, returning from parent
        if (pid == 0) {
            printf("child pid: (%d) father pid: (%d)\n", getpid(), getppid());
            if(i==0) { //first child
                start_1 = clock(); // timing
                printf("starting preemption scheduling\n"
                       "start time: %ld\n", start_1);
                check_pre = run(argumentsp, dot); //do scheduling
                end_1 = clock();
                if (check_pre == 0) {
                    printf("preemption scheduling succeed. \n"
                           "the process id is %d \n"
                           "end time: %ld\n", getpid(), end_1);
                    printf("Father process is: %d\n", getppid());
                }
            }
            if(i==1) { //second child
                start_2 = clock();
                printf("starting no preemption scheduling\n"
                       "start time: %ld\n", start_2);
                check_no_pre = run(arguments, dot); //do scheduling
                end_2 = clock();
                if (check_no_pre == 0) {
                    printf("no preemption scheduling succeed. \n"
                           "the process id is %d\n"
                           "end time: %ld\n", getpid(), end_2);
                    printf("Father process is: %d\n", getppid());
                }
            }
            exit(EXIT_SUCCESS); // child i has finished
        }
    }

    for (int i = 0; i < 2; i++) {
        waitpid(-1, &status, 0); // waiting for all children
        if (WIFEXITED(status)) {
            printf("exited, status = %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("killed by signal %d\n", WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("stopped by signal %d\n", WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("continued\n");
        }
    }


        printf("I'm parent. All children have finished.\n");
}