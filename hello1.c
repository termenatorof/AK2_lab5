// SPDX-License-Identifier: Dual BSD/GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include "hello1.h"

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello1 module with list and ktime tracking");
MODULE_LICENSE("Dual BSD/GPL");

struct hello_data {
	struct list_head list;
	ktime_t time_start;
	ktime_t time_end;
};

static LIST_HEAD(hello_list);

void print_hello(void)
{
	struct hello_data *data;
	ktime_t t_start, t_end;

	data = kmalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return;

	t_start = ktime_get();
	pr_info("Hello, world!\n");
	t_end = ktime_get();

	data->time_start = t_start;
	data->time_end = t_end;
	list_add_tail(&data->list, &hello_list);
}
EXPORT_SYMBOL(print_hello);

static int __init hello1_init(void)
{
	pr_info("Hello1 module loaded\n");
	return 0;
}

static void __exit hello1_exit(void)
{
	struct hello_data *data, *tmp;
	s64 duration;

	list_for_each_entry_safe(data, tmp, &hello_list, list) {
		duration = ktime_to_ns(ktime_sub(data->time_end, data->time_start));
		pr_info("Print duration: %lld ns\n", duration);
		list_del(&data->list);
		kfree(data);
	}
	pr_info("Hello1 module unloaded\n");
}

module_init(hello1_init);
module_exit(hello1_exit);
