// SPDX-License-Identifier: GPL

#include <linux/init.h>    /* Needed for the macros */
#include <linux/kernel.h>  /* Needed for KERN_INFO */
#include <linux/module.h>  /* Needed by all modules */
#include <net/genetlink.h> /* Needed for generic neltink */

#include <trng_module_nl.h>

SEQ seq_pool[255] = { {-1, {0} } };

int trng_module_nl_check_struct(Msg *msg)
{
	if (((msg->id != -1) && (msg->seq != -1) && (msg->flag != -1) &&
				(msg->length != -1) && (msg->seq != -1) &&
				(msg->length == 1 || msg->data != NULL))) {
		return 1;
	}

	return 0;
}

uint8_t trng_module_nl_get_seq_num(struct genl_info *info)
{
	int i;
	int number_of_elements;

	number_of_elements = (ARRAY_SIZE(seq_pool) / sizeof(seq_pool[0]));

	for (i = 0; i < number_of_elements; ++i) {
		if (seq_pool[i].seq == -1) {
			seq_pool[i].seq = i;
			seq_pool[i].info = *info;

			return i;
		}
	}

	return -1;
}

Msg trng_module_nl_process_msg(struct sk_buff *skb,
							struct genl_info *info)
{
	Msg message = {-1, -1, -1, -1, NULL};

	if (info->attrs[ATTR_CTRL_WORD_ID])
		message.id = nla_get_u8(info->attrs[ATTR_CTRL_WORD_ID]);

	if (info->attrs[ATTR_WORDS]) {
		message.data = nla_data(info->attrs[ATTR_WORDS]);
		message.length = ((nla_len(info->attrs[ATTR_WORDS]) / 4) + 1);

		printk(KERN_INFO "What I received: %d.\n", (char *)message.data);
	} else {
		message.data = NULL;
		message.length = 1;
	}

	message.seq = trng_module_nl_get_seq_num(info);
	message.flag = 0;

	return message;
}

int trng_module_nl_send_response(char *payload, int payload_size,
struct genl_info *info, struct genl_family *gen_net_app_family)
{
	struct sk_buff *new_skb;
	void *data = NULL;
	int ret = 0;
	
	printk(KERN_INFO "KM SEND: %s\n", payload);
	printk(KERN_INFO "KM SEND SIZE: %d\n", payload_size);

	new_skb = genlmsg_new(GENLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!new_skb)
		return 0;

	data = genlmsg_put(new_skb, info->snd_portid,
			info->snd_seq, gen_net_app_family,
			0, CMD_HANDLE_MSG_FROM_USER);

	if (!data)
		goto out_err;

	ret = nla_put(new_skb, ATTR_RESPONSE, payload_size, payload);
	if (ret)
		goto out_err;

	genlmsg_end(new_skb, data);

	genlmsg_reply(new_skb, info);

	return 0;

out_err:
	genlmsg_cancel(new_skb, data);
	nlmsg_free(new_skb);
	return -1;
}

void trng_module_nl_clear_seq(int seq)
{
	int i;
	int number_of_elements;

	number_of_elements = ARRAY_SIZE(seq_pool) / sizeof(seq_pool[0]);
	for (i = 0; i < number_of_elements; ++i) {
		if (seq_pool[i].seq == seq) {
			seq_pool[i].seq = -1;
			return;
		}
	}

}
