#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void n(void)
{
    system("/bin/cat /home/user/level7/.pass");
}

void m(void)
{
    puts("Nope\n");
}

int main(int argc, char **argv)
{
    char *str;
    void (*func_ptr)();

    str = malloc(0x40);
    func_ptr = malloc(sizeof(void *));
    func_ptr = &m;

    strcpy(str, argv[1]);

    func_ptr();

    return (0);
}