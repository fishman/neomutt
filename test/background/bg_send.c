/**
 * @file
 * Test code for the background message delivery
 *
 * @authors
 * Copyright (C) 2026 Reza Jelveh
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define TEST_NO_MAIN
#include "config.h"
#include "acutest.h"
#include <unistd.h>
#include "background.h"
#include "email/lib.h"
#include "test_common.h"

/**
 * spawn_child - Fork a child that exits with the given code
 * @param exit_code Exit code of the child
 */
static pid_t spawn_child(int exit_code)
{
  pid_t pid = fork();
  if (pid == 0)
  {
    sleep(1);
    _exit(exit_code);
  }
  return pid;
}

void test_bg_send(void)
{
  // int bg_send_register(pid_t pid, const char *fcc, const char *childout,
  //                      struct Email *e, SendFlags flags, const char *cmd, bool fcc_done)

  {
    // Successful delivery
    const int slot = bg_send_register(spawn_child(0), "", "/tmp/bg_send_test_childout",
                                      email_new(), 0, "sendmail", false);
    TEST_CHECK(slot >= 0);

    int reaped = 0;
    for (int i = 0; (i < 10) && (reaped == 0); i++)
    {
      sleep(1);
      reaped = bg_reap();
    }
    TEST_CHECK_NUM_EQ(reaped, 1);
    TEST_CHECK_NUM_EQ(bg_job_exit_code(slot), 0);
  }

  {
    // Failed delivery: the message is retained for a retry
    const int slot = bg_send_register(spawn_child(3), "", "/tmp/bg_send_test_childout",
                                      email_new(), SEND_REPLY, "sendmail", false);
    TEST_CHECK(slot >= 0);

    int reaped = 0;
    for (int i = 0; (i < 10) && (reaped == 0); i++)
    {
      sleep(1);
      reaped = bg_reap();
    }
    TEST_CHECK_NUM_EQ(reaped, 1);
    TEST_CHECK_NUM_EQ(bg_job_exit_code(slot), 3);
    TEST_CHECK(!bg_send_full()); // only one slot is used
  }

  {
    // Failed deliveries hold their slots until retried
    for (int i = 0; i < 9; i++)
    {
      TEST_CHECK(bg_send_register(spawn_child(1), "", "/tmp/bg_send_test_childout",
                                  email_new(), 0, "sendmail", false) >= 0);
    }

    int reaped = 0;
    for (int i = 0; (i < 10) && (reaped < 9); i++)
    {
      sleep(1);
      reaped += bg_reap();
    }
    TEST_CHECK_NUM_EQ(reaped, 9);

    TEST_CHECK(bg_send_full());
    struct Email *e = email_new();
    TEST_CHECK_NUM_EQ(bg_send_register(spawn_child(1), "", "/tmp/bg_send_test_childout",
                                       e, 0, "sendmail", false), -1);
    email_free(&e);
    TEST_CHECK_NUM_EQ(bg_job_start("exit 0"), -1);
  }

  bg_cleanup();
}
