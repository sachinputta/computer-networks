#include<stdio.h>

struct emp
{
   char hostname[100];
   char ip_add[30];
};

void main()
{
   struct emp e;
   FILE *p,*q;
   p = fopen("one.txt", "a");
   q = fopen("one.txt", "r");
   printf("Enter hostname and ip_add");
   scanf("%s %s", e.hostname, e.ip_add);
   fprintf(p,"%s %s", e.hostname, e.ip_add);
   fclose(p);
   do
   {
      fscanf(q,"%s %s", e.hostname, e.ip_add);
      printf("%s %s", e.hostname, e.ip_add);
   }
   while( !feof(q) );
   
}