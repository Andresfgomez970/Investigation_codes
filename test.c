#include <stdio.h>
#include <stdlib.h>

int SrtingLen(char * string){
  int i;
  for (i = 0; string[i] != '\0'; i++);

  return i;
}

int main(int argc, char const *argv[])
{
    printf("%d", SrtingLen("here"));    

    return 0;
}
