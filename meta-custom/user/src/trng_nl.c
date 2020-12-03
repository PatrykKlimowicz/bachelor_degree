#include <trng_nl.h>

struct trng_nl_msg {
	struct nl_sock *sk;
	struct nl_msg *msg;
	int sent;
};

static int get_msg(struct nl_msg *msg, void *attr);
static int error_res(struct sockaddr_nl *nla, struct nlmsgerr *err, void *attr);
static int on_response(struct nl_sock *sk,
				int (*err_res)(struct sockaddr_nl *,
				struct nlmsgerr *, void *),
				int (*res)(struct nl_msg *, void *),
				struct trng_nl_response *attr);

struct trng_nl_msg *trng_nl_process_msg(struct trng_nl_command cmd)
{
	struct nl_sock *sk = nl_socket_alloc();

	if (!sk) {
		printf("Cannot allocate memory for socket\n");
		return NULL;
	}

	if (genl_connect(sk)) {
		printf("Cannot connect to generic netlink\n");
		goto failed;
	}

	int family_id = genl_ctrl_resolve(sk, FAMILY_NAME);

	if (family_id < 0) {
		printf("Cannot resolve family name\n");
		goto failed;
	}


	struct nl_msg *msg = nlmsg_alloc();

	// add header
	if (!genlmsg_put(msg, 0, NL_AUTO_SEQ, family_id,
		0, 0, CMD_HANDLE_MSG_FROM_USER, 0)) {
		printf("Cannot create message header\n");
		nlmsg_free(msg);
		goto failed;
	}

	if (nla_put_u8(msg, ATTR_CTRL_WORD_ID, cmd.id)) {
		printf("Cannot add payload to message\n");
		nlmsg_free(msg);
		goto failed;
	}

	if (cmd.words && nla_put(msg, ATTR_WORDS, cmd.len * 4, cmd.words)) {
		printf("Cannot add payload to message\n");
		nlmsg_free(msg);
		goto failed;
	}

	struct trng_nl_msg *trng_msg = malloc(sizeof(struct trng_nl_msg));

	trng_msg->sk = sk;
	trng_msg->msg = msg;
	trng_msg->sent = 0;

	return trng_msg;

failed:
	nl_socket_free(sk);
	return NULL;

}

int trng_nl_send_msg(struct trng_nl_msg *msg)
{
	if (!msg) {
		printf("Message is NULL\n");
		return -1;
	}

	if (nl_send_auto(msg->sk, msg->msg) >= 0) {
		msg->sent = 1;
		return 0;
	}

	printf("Failed to send message\n");
	return -1;
}

struct trng_nl_response *trng_nl_wait_for_response(struct trng_nl_msg *msg)
{
	if (!msg || !(msg->sk) || !(msg->sent)) {
		printf("Message was not properly created or sent\n");
		return NULL;
	}

	struct trng_nl_response *res = malloc(sizeof(struct trng_nl_response));

	on_response(msg->sk, error_res, get_msg, res);
	trng_nl_drop_msg(msg);

	return res;
}

void trng_nl_print_response(struct trng_nl_response *res)
{
	for (int i = 0; i < res->len; i++)
		printf("0x%hhx ", ((char *)res->msg)[i]);

	printf("\n");

	for (int i = 0; i < res->len; i++)
		if (((char *)res->msg)[i] != '\r')
			printf("%c", ((char *)res->msg)[i]);
		else
			printf("\'\\r\'");

	printf("\n");
}

void trng_nl_drop_msg(struct trng_nl_msg *msg)
{
	if (!msg)
		return;

	if (msg->sk)
		nl_socket_free(msg->sk);

	if (msg->msg)
		nlmsg_free(msg->msg);

	free(msg);
	msg = NULL;
}

/**
 * @brief Parse message from kernel module
 *
 * @param[in] msg Message from kernel module
 * @param[out] attr Pointer to trng_nl_response struct
 * @return int NL_STOP
 */
static int get_msg(struct nl_msg *msg, void *attr)
{
	struct nla_policy policy[ATTR_MAX + 1];

	policy[ATTR_RESPONSE].type = NLA_UNSPEC;
	policy[ATTR_RESPONSE].minlen = 0;
	policy[ATTR_RESPONSE].maxlen = 0;

	struct nlattr *attrs[ATTR_MAX + 1];
	int err = genlmsg_parse(nlmsg_hdr(msg), 0, attrs, ATTR_MAX, policy);

	if (err < 0)
		return NL_STOP;

	struct genlmsghdr *header =
		(struct genlmsghdr *)(nlmsg_data(nlmsg_hdr(msg)));

	if (header->cmd != CMD_HANDLE_MSG_FROM_USER)
		return NL_STOP;

	struct trng_nl_response *res = attr;

	if (attrs[ATTR_RESPONSE]) {
		res->len = nla_len(attrs[ATTR_RESPONSE]);
		res->msg = malloc(sizeof(char) * res->len);
		memcpy(res->msg, nla_data(attrs[ATTR_RESPONSE]), res->len);
	}

	return NL_STOP;
}

static int error_res(struct sockaddr_nl *nla, struct nlmsgerr *err, void *attr)
{
	*((int *)attr) = -1;
	return NL_STOP;
}

/* handle the message from module */
static int on_response(struct nl_sock *sk,
				int (*err_res)(struct sockaddr_nl *,
				struct nlmsgerr *, void *),
				int (*res)(struct nl_msg *, void *),
				struct trng_nl_response *attr)
{
	int err = 0;

	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);

	/* set response for proper messages */
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, res, attr);
	/* set response for errors */
	nl_cb_err(cb, NL_CB_CUSTOM, err_res, (void *)(&err));

	int ret = nl_recvmsgs(sk, cb);

	free(cb);
	if (err)
		return err;
	return ret;
}
