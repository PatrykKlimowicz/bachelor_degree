#ifndef __TRNG_MODULE_NL_H__
#define __TRNG_MODULE_NL_H__

#include <linux/init.h>    /* Needed for the macros */
#include <linux/kernel.h>  /* Needed for KERN_INFO */
#include <linux/module.h>  /* Needed by all modules */
#include <net/genetlink.h> /* Needed for generic neltink */
#include <stdbool.h>

#include <trng_module_shared_nl.h>

/**
 * @brief SEQ contains sequence number and its owner 
 * 
 */
typedef struct SEQ
{
    int seq;
    struct genl_info info;
} SEQ;

/**
 * @brief structure that contains neccessary data to create message.
 * 
 */
typedef struct Msg
{
    uint8_t id;
    uint8_t seq;
    uint8_t flag;
    uint8_t length;
    void *data;
} Msg;

/**
 * @brief parse netlink message from user
 * 
 * @param skb message buffer
 * @param info info about generic netlink socket
 * @return Msg structure with all nedded data
 */
Msg trng_module_nl_process_msg(struct sk_buff *skb, struct genl_info *info);

/**
 * @brief send response to user
 * 
 * @param payload message to send
 * @param payload_size size of payload
 * @param info  info about generic netlink socket
 * @return int  error code
 */
int trng_module_nl_send_response(char *payload, int payload_size, struct genl_info *info, struct genl_family *gen_net_app_family);

/**
 * @brief generate sequence number for message
 * 
 * @param info info about generic netlink socket - owner
 * @return uint8_t seq num
 */
uint8_t trng_module_nl_get_seq_num(struct genl_info *info);

/**
 * @brief free sequence number.
 * 
 */
void trng_module_nl_clear_seq(int seq);
/**
 * @brief check if struct is filled properly
 * 
 * @param msg structure with message info
 * @return int 1 as true, 0 as false
 */
int trng_module_nl_check_struct(Msg *msg);

#endif /* __TRNG_MODULE_NL_H__ */
