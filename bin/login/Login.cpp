/*
 * Copyright (C) 2015 Niek Linnenbank
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Login.h"

Login::Login(int argc, char **argv)
    : POSIXApplication(argc, argv)
{
    parser().setDescription("Login program which starts the user shell");
    parser().registerPositional("INPUT", "Input character device for reading standard input");
    parser().registerPositional("OUTPUT", "Output character device for writing standard output");
}

Login::~Login()
{
}

Login::Result Login::exec()
{
    const char *sh_argv[] = { "/bin/sh", 0 };
    pid_t pid;
    int status;

    // Wait until the I/O files are available (busy loop)
    while (true)
    {
        // Re-open standard I/O
        close(0);
        close(1);
        close(2);

        // Stdin
        if (open(arguments().get("INPUT"), O_RDONLY) == -1)
            continue;

        // Stdout
        if (open(arguments().get("OUTPUT"), O_RDWR) == -1)
            continue;

        // Stderr
        if (open(arguments().get("OUTPUT"), O_RDWR) == -1)
            continue;

        // Done
        break;
    }

    // Loop forever with a login prompt
    while (true)
    {
        // Start the shell
        if ((pid = forkexec("/bin/sh", sh_argv)) != (pid_t) -1)
        {
            waitpid(pid, &status, 0);
        }
        else
        {
            ERROR("forkexec '" << sh_argv[0] << "' failed: " << strerror(errno));
        }
    }
    return Success;
}
