#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
	 int len;
	 char filename[maxlen +1];
	 /*Create a file*/
	 if(Create("text.txt") == -1)
	 {
	 	Print("Error");
	 }
	 else
	 {
	 	Print("Successful");
	 }
	 Halt();
}