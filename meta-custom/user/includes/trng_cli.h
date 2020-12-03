#ifndef __TRNG_CLI_H__
#define __TRNG_CLI_H__

#include <ctype.h>
#include <getopt.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <trng_nl.h>
#include <trng_module_shared_nl.h>

#define PRINT_FLAG 0x2
#define COMMAND_FLAG 0x4

/**
 * @brief Read options, set flags and parse command.
 *
 * @param[in] argc argc from main
 * @param[in] argv argv from main
 * @param[out] flags Pointer to variable that will contain program flags. Flags can be read by using & operator.
 * @param[out] cmd pointer to structure that will contain command that can be send to KM
 * @return int -1 if failed, otherwise 0
 */
int trng_cli_read_opts(int argc, char **argv, int *flags, struct trng_nl_command *cmd);

#endif /* __TRNG_CLI_H__ */
