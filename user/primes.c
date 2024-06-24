#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFF_SIZE 128
int is_prime(int n)
{
    if (n <= 1)
    {
        return 0;
    }
    if (n <= 3)
    {
        return 1;
    }
    if (n % 2 == 0 || n % 3 == 0)
    {
        return 0;
    }
    for (int i = 5; i * i <= n; i = i + 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int main(int number_of_checkers)
{
    while (1)
    {
        // this is the generator code:
        int cd1 = channel_create();
        int cd2 = channel_create();
        int pid = -1;
        for (int i = 0; i < number_of_checkers; i++)
        {
            pid = fork();
            if (pid == 0)
            {
                // this is the checker code:
                int n;
                while (1)
                {
                    if (channel_take(cd1, &n) < 0)
                    {
                        channel_destroy(cd1);
                        printf("Checker #%d failed taking, PID: %d.\n", i, pid);
                        exit(0);
                    }
                    if (is_prime(n))
                    {
                        if (channel_put(cd2, n) < 0)
                        {
                            channel_destroy(cd1);
                            printf("Channel 2 unavailable. Exiting checker #%d, PID: %d.\n", i, pid);
                            exit(0);
                        }
                    }
                }
            }
        }
        if (fork() == 0)
        {
            // this is the printer code:
            int n;
            int counter = 1;
            while (counter <= 100)
            {
                if (channel_take(cd2, &n) < 0)
                {
                    channel_destroy(cd2);
                    printf("Printer failed taking, PID: %d.\n", pid);
                    exit(0);
                }
                printf("%d. %d\n", counter, n);
                counter++;
            }
            channel_destroy(cd2);
            printf("Reached 100 primes. Exiting printer, PID: %d.\n", pid);
            exit(0);
        }
        // this is the main code:
        int i = 2;
        while (1)
        {
            if (channel_put(cd1, i) < 0)
            {
                while (wait(0) > 0)
                    ;
                printf("All child processes have exited.\n");
                break;
            }
            i++;
        }

        char buff[BUFF_SIZE];

        while (buff[0] != 'y' || buff[0] != 'Y' || buff[0] != 'n' || buff[0] != 'N')
        {
            printf("Do you want the whole thing again? [y / n]\n");
            gets(buff, BUFF_SIZE);
            if (buff[0] == 'y' || buff[0] == 'Y')
            {
                break;
            }
            else if (buff[0] == 'n' || buff[0] == 'N')
            {
                printf("Ciao!\n");
                exit(0);
            }
        }
    }

    return 0;
}