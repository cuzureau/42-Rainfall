#include <string.h>
#include <stdlib.h>

int language = 0;

int		greetuser(char *name) {
	char str[72];

	switch (language) {
		case 0:
			strcpy(str, "Hello ");
			break;
		case 1:
			strcpy(str, "Hyvää päivää ");
			break;
		case 2:
			strcpy(str, "Goedemiddag! ");
			break;
		default:
			break;
	}
	strcat(str, name);
	return (puts(str));
}

int		main(int ac, char **av) {
	char str[76];
	char str2[80];
	char *lang;

	if (ac != 3)
		return 1;

	memset(str, '\0', 19);

	stpncpy(str, av[1], 40);
	strncpy(str + 40, av[2], 32);
	lang = getenv("LANG");
	if (*lang) {
		if (!memcmp(lang, "fi", 2)) {
			language = 1;
		} else {
			if (!memcmp(lang, "nl",2)) {
				language = 2;
			}
		}
	}
	for (int i = 0; i < 19; i++) {
		str2[i] = str[i];
	}

	return greetuser(str2);
}