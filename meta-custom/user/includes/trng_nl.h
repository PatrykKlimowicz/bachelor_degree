#include <stdint.h>

#include <netlink/attr.h>
#include <netlink/cache.h>
#include <netlink/data.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>
#include <netlink/route/link.h>
#include <netlink/socket.h>

#include <trng_module_shared_nl.h>
#include <trng_cli.h>

#ifndef __TRNG_NL_H__
#define __TRNG_NL_H__

/**
 * @brief structure representing command that
 * can be used for communication with SPI device's driver
 *
 */
struct trng_nl_command {
	uint8_t id;     /**< command id, check trng_module_nl.h for possible values */
	uint8_t len;    /**< length of words */
	char *words;    /**< command payload */
};

/**
 * @brief structure representing response
 * from KM
 *
 */
struct trng_nl_response {
	size_t len;     /**< lenght of response */
	void *msg;      /**< response */
};

/**
 * @brief structure representing message ready to be send to KM
 *
 */
struct trng_nl_msg;

/**
 * @brief creat message with command
 *
 * @param cmd command
 * @return struct trng_nl_msg* message that can be send to KM
 */
struct trng_nl_msg *trng_nl_process_msg(struct trng_nl_command cmd);

/**
 * @brief send message to KM
 *
 * @param msg message created by trng_nl_process_msg function
 * @return int -1 if failed, otherwise 0
 */
int trng_nl_send_msg(struct trng_nl_msg *msg);

/**
 * @brief Wait for response from KM
 * Function allocates memory for response, so it should be released afterwards (msg field and returned pointer to trng_nl_response struct)
 *
 * @param msg Message that was sent to KM
 * @return struct trng_nl_response* Response from KM or NULL if msg struct doesn't contain necessary data or message wasn't send to KM
 */
struct trng_nl_response *trng_nl_wait_for_response(struct trng_nl_msg *msg);

/**
 * @brief Print bytes of response as hex and char
 *
 * @param res Response from KM
 */
void trng_nl_print_response(struct trng_nl_response *res);

/**
 * @brief Release memory allocated for message
 *
 * @param msg
 */
void trng_nl_drop_msg(struct trng_nl_msg *msg);

#endif /* __TRNG_NL_H__ */
