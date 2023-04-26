#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>

void signal_handler(int s)
{
    printf("Floating point exception detected!\n");
    void *callstack[128];
    int i, frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    for (i = 0; i < frames; ++i) {
        printf("%s\n", strs[i]);
    }
    free(strs);
    exit(EXIT_FAILURE);
}

int super_safe_div(int a, int b) {
    return a/b;
}

int main(void)
{
    signal(SIGFPE, signal_handler);

    printf("%d\n", super_safe_div(42, 0));

    return EXIT_SUCCESS;
}
