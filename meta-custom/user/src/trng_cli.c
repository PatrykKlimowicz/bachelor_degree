#include <trng_cli.h>

/* parse options given to application */
int trng_cli_read_opts(int argc, char **argv,
int *flags, struct trng_nl_command *cmd)
{
	if (!cmd)
		return -1;

	const struct option long_opts[] = {
		{"print", no_argument, 0, 'p' },
		{"command", required_argument, 0, 'c'},
		{NULL, 0, NULL, 0}
	};

	int c;
	char *end;

	*flags = 0;

	/* reading options */
	while ((c = getopt_long(argc, argv, "pc:", long_opts, NULL)) != -1)
		switch (c) {
		case 'p': /* print */
			*flags |= PRINT_FLAG;
			break;

		case 'c': /* command */
			*flags |= COMMAND_FLAG;

			if (strcmp(optarg, "alive") == 0) {
				cmd->id = (uint8_t)ALIVE;
				cmd->len = 0;
			} else if (strcmp(optarg, "get_random") == 0) {
				cmd->id = (uint8_t)GET_RANDOM;
				cmd->len = 1;
			} else if (strcmp(optarg, "set_led_mode") == 0) {
				cmd->id = (uint8_t)SET_LED_MODE;
				cmd->len = 1;
			} else if (strcmp(optarg, "get_led_mode") == 0) {
				cmd->id = (uint8_t)GET_LED_MODE;
				cmd->len = 0;
			} else if (strcmp(optarg, "test_set_random_mode") == 0) {
				cmd->id = (uint8_t)TEST_SET_RANDOM_MODE;
				cmd->len = 1;
			} else if (strcmp(optarg, "test_set_random_pattern") == 0) {
				cmd->id = (uint8_t)TEST_SET_RANDOM_PATTERN;
				cmd->len = 1;
			} else {
				printf("%s is not a command name\n", optarg);
				return -1;
			}

			/* reading additional pararameters */
			if (cmd->len >= 1) {
				cmd->words = (char *)calloc(4 * (cmd->len) + 1, sizeof(char));
				int i = 0;

				for (i = 0; i < cmd->len && optind < argc && argv[optind][0] != '-'; i++) {
					uint32_t word = strtol(argv[optind], &end, 0);

					/* validating data */
					if (cmd->id == GET_RANDOM &&
						(word >= 256 || word < 1)) {
						printf("Max number of random bytes is 255. Minimum is 1\n");
						free(cmd->words);
						return -1;
					} else if (cmd->id == SET_LED_MODE &&
							(word > 3 || word < 0)) {
						printf("%d is not a valid led mode\n", word);
						free(cmd->words);
						return -1;
					} else if (cmd->id == TEST_SET_RANDOM_MODE &&
							(word > 1 || word < 0)) {
						printf("%d is not a valid random mode\n", word);
						free(cmd->words);
						return -1;
					} else if (end == argv[optind]) {
						printf("\"%s\" is not an integer\n", argv[optind]);
						free(cmd->words);
						return -1;
					}

					int n = sizeof word + 1;

					for (int y = 0; n-- > 0; y++)
						cmd->words[4 * i + y] = (word >> ((sizeof(word) - n) * 8)) & 0xff;

					optind++;
				}

				cmd->words[4 * i] = '\0';

				if (i < cmd->len) {
					printf("Too few arguments\n");
					free(cmd->words);
					return -1;
				}

			} else    /* cmd->len >= 1 */
				cmd->words = NULL;

			if (optind < argc && argv[optind][0] != '-') {
				printf("Too many arguments\n");
				free(cmd->words);
				return -1;
			}
			break;

		default:
			printf("wrong option\n");
			return -1;
		}

	/* if no command, print help */
	if (!(*flags & COMMAND_FLAG))
		printf("Usage:\t%s [OPTIONS]\n\t"
				   "-p, --print\tprint response from KM\n\t"
				   "-c, --command\tsend command to kernel module\n\n"
				   "Commands:\n"
				   "alive\t\t\tno args, check if device is alive\n"
				   "get_random\t\t1. arg - number of bytes, return n random bytes\n"
				   "set_led_mode\t\t1. arg - led mode, change led mode\n"
				   "\t\t\tModes:\n"
				   "\t\t\t\t0 - off\n"
				   "\t\t\t\t1 - on\n"
				   "\t\t\t\t2 - blink on message\n"
				   "\t\t\t\t3 - blink on random\n"
				   "get_led_mode\t\tno args, return led mode\n"
				   "test_set_random_mode\t1. arg - random mode, change random mode\n"
				   "\t\t\tModes:\n"
				   "\t\t\t\t0 - regular mode\n"
				   "\t\t\t\t1 - pattern mode\n"
				   "test_set_random_pattern\t1. arg - random pattern, change random pattern\n",
				basename(argv[0]));

	return 0;
}
