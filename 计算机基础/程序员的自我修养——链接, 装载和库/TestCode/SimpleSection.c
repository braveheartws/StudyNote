#include <stdio.h>

//Linux
// gcc -c SimpleSection.c
//windows
// 

int global_init_var = 84;
int global_uninit_var;

printf(const char* format, ...);

void func1(int i) {
	printf("%d\n", i);
}

int main(int argc, char const *argv[])
{
	static int static_var = 85;
	static int static_var2;

	int a = 1;
	int b;
	func1(static_var + static_var2 + a + b);
	return a;
}