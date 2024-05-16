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

#ifndef _APPLICATION_OPTIONS_H
#define _APPLICATION_OPTIONS_H

#include <stdbool.h>

/* Application options, specified on the command line */

typedef struct {
	bool listening;                /* Enable xtea messages */
	const char *topic;             /* Topic for sharing messages */
	const char *server_host;       /* Server host default = test.mosquitto.org */
	unsigned int server_port;      /* Server port default = 1883 */
	const char *server_user;       /* Server auth user */
	const char *server_pwd;        /* Server auth password */
} application_options_t;

extern application_options_t runtime_options;

#endif
