// SPDX-License-Identifier: GPL
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <linux/spi/spidev.h>
#include <linux/spi/spi.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <net/genetlink.h>
#include <stdbool.h>

#include <trng_module_nl.h>
#include <trng_module_spi.h>
#include <trng_module_sim.h>

#define WQ_NAME "wq_embedded"

static int IRQ_PIN = 21;
static unsigned int irq_number;
static int FLAG;

module_param(FLAG, int, 0444);
void (*send)(Msg *msg);
void (*receive)(Msg *msg, void *buf);
static struct spi_device *spi_dev;
Msg message;
struct workqueue_struct *wqueue;

static int supervisor(struct sk_buff *skb, struct genl_info *info);
static void handle_msg_from_dev(struct work_struct *task);

static struct nla_policy app_policy[ATTR_MAX + 1] = {
	[ATTR_CTRL_WORD_ID] = {.type = NLA_U8},
	[ATTR_WORDS] = {.type = NLA_UNSPEC},
	[ATTR_RESPONSE] = {.type = NLA_UNSPEC},
};

static struct genl_ops gen_net_app_ops[] = {
	{.cmd = CMD_HANDLE_MSG_FROM_USER,
	 .policy = app_policy,
	 .doit = supervisor},
};

static struct genl_family gen_net_app_family = {
	.module = THIS_MODULE,
	.name = FAMILY_NAME,
	.version = 1,
	.maxattr = ATTR_MAX,
	.ops = gen_net_app_ops,
	.n_ops = ARRAY_SIZE(gen_net_app_ops),
};

typedef struct Dev_id {
	struct genl_info *info;	
} Dev_id;

typedef struct my_work_struct {
    struct work_struct work;    
	struct genl_info *info;
} my_work_struct;

static void handle_msg_from_dev(struct work_struct *tasks)
{
	my_work_struct *my_work = (my_work_struct *)tasks;
	unsigned char buf[BUF_LEN] = {0};
	trng_module_spi_receive(&message, (void *)buf);

	if (!message.length)
		return;

	trng_module_nl_send_response(message.data,
				message.length, my_work->info, &gen_net_app_family);

	trng_module_nl_clear_seq(message.seq);
	flush_workqueue(wqueue);
}

irqreturn_t irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	my_work_struct task;
	printk(KERN_INFO "PRZERWANIE WYWOLANE \n");
	task.info = ((struct Dev_id *)dev_id)->info;
	INIT_WORK((struct work_struct *)&task, handle_msg_from_dev);

    queue_work(wqueue, (struct work_struct *)&task);
    return IRQ_HANDLED;
}

static int supervisor(struct sk_buff *skb, struct genl_info *info)
{
	message = trng_module_nl_process_msg(skb, info);
	Dev_id *dev_id = kmalloc(sizeof(Dev_id), GFP_KERNEL);
	(send)(&message);
	if (trng_module_nl_check_struct(&message)) {
		if (receive) {
			unsigned char buf[BUF_LEN] = {0};
			(receive)(&message, (void *)buf);

			trng_module_nl_send_response(message.data,
				message.length, info, &gen_net_app_family);

			trng_module_nl_clear_seq(message.seq);
		} else {
			dev_id->info = info;
			gpio_request(IRQ_PIN, " ");
			gpio_direction_input(IRQ_PIN);

			irq_number = gpio_to_irq(IRQ_PIN);
			if(irq_number > 0)
				if (request_irq(irq_number,
						(irq_handler_t)irq_handler,
						IRQ_TYPE_EDGE_RISING,
						"interrupt_from_slave", (void *)dev_id) < 0)
					return -EIO;
		}
	} else {
		trng_module_nl_send_response("ERROR\n", strlen("ERROR\n"), info,
		&gen_net_app_family);

		trng_module_nl_clear_seq(message.seq);
	}

	kfree(dev_id);
	return 0;
}

static int __init km_init(void)
{
	int err;
	struct spi_master *master;

	struct spi_board_info spi_device_info = {
        .modalias = "our_spi",
        .max_speed_hz = 3 * 1000 * 1000,
        .bus_num = MY_BUS_NUM,
        .chip_select = 0,
        .mode = 2,
    };

	if (!FLAG) {
		send = trng_module_sim_send;
		receive = trng_module_sim_receive;
	} else {
		master = spi_busnum_to_master(spi_device_info.bus_num);
		if (!master) {
			printk(KERN_INFO "MASTER not found.\n");
			return -ENODEV;
		}

		spi_dev = spi_new_device(master, &spi_device_info);
		if (!spi_dev) {
			printk(KERN_INFO "FAILED to create slave.\n");
			return -ENODEV;
		}
		spi_dev->bits_per_word = 8;

		err = spi_setup(spi_dev);
		if (err) {
			printk(KERN_INFO "FAILED to setup slave.\n");
			spi_unregister_device(spi_dev);
			return -ENODEV;
		}

		register_spi_dev(spi_dev);

		wqueue = create_singlethread_workqueue(WQ_NAME);

		send = trng_module_spi_send;
		receive = NULL;
	}

	err = genl_register_family(&gen_net_app_family);
	if (err)
		return 1;

	return 0;
}

static void __exit km_exit(void)
{
	if (FLAG) {
		gpio_free(IRQ_PIN);
		free_irq(irq_number, NULL);
		spi_unregister_device(spi_dev);
		destroy_workqueue(wqueue);
	}

	genl_unregister_family(&gen_net_app_family);
}

postcore_initcall(km_init);
module_exit(km_exit);

MODULE_LICENSE("GPL");
