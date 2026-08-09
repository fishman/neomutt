/**
 * @file
 * Run external commands in the background
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

#ifndef MUTT_BACKGROUND_H
#define MUTT_BACKGROUND_H

#include "send/lib.h"

void bg_cleanup(void);
void bg_init(void);
int bg_job_exit_code(int slot);
int bg_job_start(const char *cmd);
int bg_reap(void);
bool bg_send_full(void);
int bg_send_register(pid_t pid, const char *fcc, const char *childout,
                     struct Email *e, SendFlags flags, const char *cmd, bool fcc_done);
void bg_send_retry(int slot);
void bg_start_command(void);
void bg_wait(void);
void dlg_output(void);

#endif /* MUTT_BACKGROUND_H */
