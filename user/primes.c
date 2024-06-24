#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

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
    // this is the generator code:
    int cd1 = channel_create();
    int cd2 = channel_create();
    for (int i = 0; i < number_of_checkers; i++)
    {
        if (fork() == 0)
        {
            // this is the checker code:
            int n;
            while (1)
            {
                if (channel_take(cd1, &n) < 0)
                {
                    channel_destroy(cd1);
                    exit(0);
                }
                if (is_prime(n))
                {
                    if (channel_put(cd2, n) < 0)
                    {
                        channel_destroy(cd1);
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
                exit(0);
            }
            printf("%d. %d\n", counter, n);
            counter++;
        }
        channel_destroy(cd2);
        exit(0);
    }
    // this is the main code:
    int i = 2;
    while (1)
    {
        if (channel_put(cd1, i) < 0)
        {
            printf("ask user if he want to do again or exit\n");
        }
        i++;
    }

    return 0;
}