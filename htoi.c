#include <stdio.h>

/*
 * alphanumeric to integer
 */
int atoi(const char *s)
{
	int i;
	int n = 0;

	for (i = 0; s[i] >='0' && s[i] <= '9'; i++)
		n = 10 * n + (s[i] - '0');

	return n;
}

int tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return c + 'a' - 'A';
	else
		return c;
}

unsigned int htoi(const char *s)
{
	int i = 0;
	unsigned int n = 0;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		i = 2;

	for (; (s[i]>='0' && s[i]<='9') || (s[i]>='a' && s[i]<='z') || (s[i]>='A' && s[i]<='Z'); i++) {
		if (tolower(s[i]) > '9')
			n = 16 * n + (10 + tolower(s[i]) - 'a');
		else
			n = 16 * n + (tolower(s[i]) - '0');
	}

	return n;
}

int main(int argc, char **argv)
{
	int i;
	unsigned int ret;

	printf("argc = %d\n", argc);
	for (i = 0; i < argc; i++)
		printf("argv[%d] = %s\n", i, argv[i]);

	ret = htoi(argv[1]);
	printf("argv[1] => 0x%08x\n", ret);

	ret = htoi(argv[2]);
	printf("argv[2] => 0x%08x\n", ret);

	return 0;
}
