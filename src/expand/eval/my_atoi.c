#include <stdio.h>

int my_atoi(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
    {
        len += 1;
    }

    int n = 0;
    int k = 1;
    int signe = 0;
    int min = 1;
    int h = 0;

    while (str[h] == ' ')
    {
        h += 1;
    }

    if (str[h] == '-' || str[h] == '+')
    {
        signe = 1;
        if (str[h] == '-')
        {
            min = -1;
        }
    }

    for (int j = len - 1; j >= signe + h; j -= 1)
    {
        if (str[j] < '0' || str[j] > '9')
        {
            return 0;
        }

        n += (str[j] - 48) * k;
        k *= 10;
    }
    return min * n;
}
