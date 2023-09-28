#include <stdio.h>
#include <stdlib.h>

int m = 0;

void v(void)
{
    char buffer[0x200];

    fgets(buffer, 0x200, stdin);
    printf(buffer);
    if (m != 0x40)
        return;
    fwrite("Wait what?!\n", 0xc, 1, stdout);
    system("/bin/sh");
}

int main(void)
{
    v();
    return (0);
}