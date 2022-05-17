#include "revert_string.h"

void RevertString(char *str)
{
	int len = 0;
	while (*(str + len) != '\0') len++;
	
	char tmp;
	for (int i = 0; i < len/2; i++) {
		tmp = str[i];
		str[i] = str[len-1-i];
		str[len-1-i] = tmp;
	}
}

