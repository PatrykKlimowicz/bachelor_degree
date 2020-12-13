// SPDX-License-Identifier: GPL

#include <linux/random.h>

#include <trng_module_nl.h>

static int RAND_MODE;
static int RAND_LEN;
static unsigned char RAND_PATTERN[5] = {0};

static char LED_MODE;

void trng_module_sim_send(Msg *msg)
{
	/*
	 * Fake function. Does nothing,
	 * but supervisor 'thinks' that he sends
	 * message via SPI to device.
	 */
}

/**
 * @brief private function used only when user
 * command is get random and random mode is set to 0.
 * Fills buffer with random bytes
 *
 * @param buf buffer to fill in with random bytes
 * @param length length of generated random
 */
static void generate_random(unsigned char *buf, int length)
{
	get_random_bytes(buf, length);
}

/**
 * @brief private function used only when user
 * command is get random and random mode is set to 1.
 * Fills buffer with random pattern.
 *
 * @param msg Msg structure which data will be equal to filled buffer
 * @param buf buffer to fill in with random pattern
 * @param length length of generated random
 */
static void fill_buf_with_pattern(Msg *msg, unsigned char *buf, int length)
{
	int i, j;

	for (i = 0, j = 3; i < length; ++i, --j) {
		buf[i] = RAND_PATTERN[j];
		if (j == 0)
			j = 4;
	}

	buf[length] = '\0';
	msg->data = buf;
	msg->length = length;
}

void trng_module_sim_receive(Msg *msg, void *buf)
{
	int random_mode;
	int buf_size;
	int length;
	char *response_to_user;
	int i;

	switch (msg->id) {
	case ALIVE:
		response_to_user = "ALIVE";
		msg->data = (void *)response_to_user;
		msg->length = strlen(response_to_user);
		break;
	case GET_RANDOM:
		;
		buf_size = 256;
		length = ((int *)msg->data)[0];

		if (!RAND_MODE) {
			if (((length % 4 == 0) && (length > buf_size - 1))) {
				response_to_user = "Wrong length of random number";
				msg->data = (void *)response_to_user;
				msg->length = strlen(response_to_user);
			} else {
				printk(KERN_INFO "Length in generate_rand is: %d", length);
				generate_random(buf, length);
				((char *)buf)[length] = '\0';
				msg->data = buf;
				msg->length = length;
			}
		} else {
			fill_buf_with_pattern(msg, buf, length);
		}
		break;
	case SET_LED_MODE:
		LED_MODE = *((char *)msg->data);
		response_to_user = "Set mode";
		msg->data = (void *)response_to_user;
		msg->length = strlen(response_to_user);
		break;
	case GET_LED_MODE:
		response_to_user = &LED_MODE;
		msg->data = (void *)response_to_user;
		msg->length = 1;
		break;
	case TEST_SET_RANDOM_MODE:
		;
		random_mode = *((int *)msg->data);

		if (random_mode) {
			response_to_user = "Random mode set - 1.";
			msg->data = (void *)response_to_user;
			msg->length = strlen(response_to_user);
		} else {
			response_to_user = "Random mode set - 0.";
			msg->data = (void *)response_to_user;
			msg->length = strlen(response_to_user);
		}
		RAND_MODE = random_mode;
		break;
	case TEST_SET_RANDOM_PATTERN:
		;
		RAND_LEN = 4;

		for (i = 0; i < RAND_LEN; ++i)
			RAND_PATTERN[i] = ((unsigned char *)msg->data)[i];

		RAND_PATTERN[4] = '\0';

		response_to_user = "Set pattern";
		msg->data = (void *)response_to_user;
		msg->length = strlen(response_to_user);
		break;
	default:
		response_to_user = "Wrong id\n";
		msg->data = (void *)response_to_user;
		msg->length = strlen(response_to_user);
		break;
	}
}
