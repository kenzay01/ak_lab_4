
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

struct TimingList {
  struct TimingList *next;
  ktime_t t;
};

static unsigned int msg_count = 1;
static struct TimingList *tl_head;

static void init_new_list(void)
{
  tl_head = kmalloc(sizeof(struct TimingList), GFP_KERNEL);

  tl_head->next = NULL;
  tl_head->t = ktime_get();
}

static void insert_new_time(void)
{
  struct TimingList *last_tl_node = tl_head;
  struct TimingList *new_tl_node = kmalloc(sizeof(struct TimingList),
            GFP_KERNEL);

  while (last_tl_node->next != NULL)
    last_tl_node = last_tl_node->next;


  new_tl_node->next = NULL;
  new_tl_node->t = ktime_get();

  last_tl_node->next = new_tl_node;
}

static void print_time(ktime_t *t)
{
  printk(KERN_EMERG "%lld\n", *t);
}

static void destroy_list(void)
{
  struct TimingList *curr_tl = tl_head;
  struct TimingList *next_tl = NULL;

  while (curr_tl != NULL) {
    print_time(&(curr_tl->t));

    next_tl = curr_tl->next;
    kfree(curr_tl);
    curr_tl = next_tl;
  }
}

static void update_timing_list(void)
{
  if (tl_head == NULL)
    init_new_list();
  else
    insert_new_time();
}

static int __init hello_init(void)
{
  int i;

  if (msg_count > 10) {
    printk(KERN_EMERG "Error: msg_count too large (%d > 10)\n",
        msg_count);
    return -EINVAL;
  }

  if (msg_count == 0 || msg_count >= 5) {
    printk(KERN_EMERG "Warning: msg_count = %d "
        "(msg_count == 0 or 5 <= msg_count <= 10)\n",
        msg_count);
  }

  for (i = 0; i < msg_count; i++) {
    update_timing_list();

    printk(KERN_EMERG "Hello, world!\n");
  }

  return 0;
}

static void __exit hello_exit(void)
{
  destroy_list();
}

module_param(msg_count, uint, 0444);
MODULE_PARM_DESC(msg_count, "Amount of hello world outputs on load");

module_init(hello_init);
module_exit(hello_exit);
