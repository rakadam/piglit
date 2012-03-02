#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main(int argc, char *argv[])
{
  size_t size;
  FILE *f, *f2;
  char buf[60000];
  char *obuf;
  char *text;
  int i;
  
  
  assert(argc == 2);
  f = fopen(argv[1], "r");
  assert(f);
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  rewind(f);
  
  text = malloc(size);
  fread(text, 1, size, f);
  
  sprintf(buf, "%s.c", argv[1]);
  
  for (i = strlen(buf)-1; i >= 0; i--)
  {
    if (buf[i] == '/')
    {
      obuf = &buf[0] + i + 1; 
      break;
    }
  }
  
  f2 = fopen(obuf, "w");
  assert(f2);
  
  fprintf(f2, "const char* opencl_kernel_file = \n");
  
  fprintf(f2, "\"");
  
  for (i = 0; i < size; i++)
  {
    if (text[i] == '\n')
    {
      fprintf(f2, "\\n\"\n\"");
    }
    else
    {
      fprintf(f2, "%c", text[i]);
    }
  }
  
  fprintf(f2, "\";\n");
  
  free(text);
  fclose(f);
  fclose(f2);
  
  return 0;
}
