#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>

char** global_argv;
int counter = 0;

char* fpe_error(int code);

void signal_handler(int s, siginfo_t* info, void* ucontext)
{
    printf("Floating point exception detected!\n");

    counter++;
    printf("counter = %d\n", counter);
    
    char* type = fpe_error(info->si_code); 

    openlog("sigfpe", LOG_PID, LOG_USER);

    syslog(
            LOG_ERR,
            "Floating point exception of type \"%s\" detected!\n", 
            type);

    closelog();

    printf("Error code: %s\n", type);

    printf("Error location: %p\n", info->si_addr);

    printf("\nRestarting the process");
    printf("------------------------------------------\n\n");

    // Em uma aplicação escrita em C,
    // o primeiro argumento é o executável
    execv(global_argv[0], global_argv);

    // Só vamos executar essa parte do código se o execv falhar
    exit(EXIT_FAILURE);
}

int super_safe_div(int a, int b) 
{
    return a/b;
}

int main(int argc, char* argv[])
{
    global_argv = argv;

    struct sigaction action; 
    action.sa_flags = SA_SIGINFO | SA_NODEFER;
    action.sa_sigaction = signal_handler;

    printf("My pid is: %d\n", getpid());

    int error = sigaction(SIGFPE, &action, NULL);
    if (error) {
        printf("FAILED: %d\n", error);
        exit(EXIT_FAILURE);
    }

    // Aqui nós dividimos por zero
    printf("%d\n", super_safe_div(42, 0));

    // Essa parte do código só será executada se a
    // a divisão por zero for um sucesso
    return EXIT_SUCCESS;
}


char* fpe_error(int code) {
    switch (code) {
        case FPE_INTDIV:    
            return "Integer divide by zero.";
        case FPE_INTOVF:
            return "Integer overflow.";
        case FPE_FLTDIV:
            return "Floating-point divide by zero.";
        case FPE_FLTOVF:
            return "Floating-point overflow.";
        case FPE_FLTUND:
            return "Floating-point underflow.";
        case FPE_FLTRES:
            return "Floating-point inexact result.";
        case FPE_FLTINV:
            return "Floating-point invalid operation.";
        case FPE_FLTSUB:
            return "Subscript out of range.";
    }
    return "No idea what code that is!";
}
