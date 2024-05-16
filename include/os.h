/*
 *  ncmqtt - netcat over mqtt
 *
 *  Copyright (C) 2023       Manuele Conti (manuele.conti@archimede-energia.com)
 *  Copyright (C) 2023       Alessio Antoniazza (alessio.antoniazza@archimede-energia.com)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */

#ifndef _OS_H
#define _OS_H

#ifdef __linux__
#include <pty.h>  // Inclusione di pty.h su sistemi Linux
#elif __APPLE__
#include <util.h> // Inclusione di util.h su sistemi macOS
#else
#error "Sistema operativo non supportato"
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <termios.h>
#include <termcap.h>
#include <fcntl.h>
#include <time.h>


#endif
