#include <ctype.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

#include <netlink/attr.h>
#include <netlink/cache.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>
#include <netlink/route/link.h>
#include <netlink/socket.h>

#include <string.h>

#include <trng_cli.h>
#include <trng_nl.h>

int main(int argc, char **argv)
{

	int flags = 0;
	struct trng_nl_command cmd;

	if (trng_cli_read_opts(argc, argv, &flags, &cmd))
		return 0;

	if (flags & COMMAND_FLAG) {

		struct trng_nl_msg *msg = trng_nl_process_msg(cmd);

		if (msg == NULL) {
			if (cmd.words != NULL)
				free(cmd.words);
			return -1;
		}

		if (trng_nl_send_msg(msg)) {
			if (cmd.words != NULL)
				free(cmd.words);
			return -1;
		}

		struct trng_nl_response *res = trng_nl_wait_for_response(msg);

		if (res == NULL)
			printf("Could not get a response from KM\n");
		else {
			if (flags & PRINT_FLAG && res != NULL)
				trng_nl_print_response(res);
			free(res->msg);
			free(res);
		}

		if (cmd.words != NULL)
			free(cmd.words);
	}



	return 0;
}
