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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>

#include "MQTTClient.h"
#include "application_options.h"
#include "version.h"

// Constants
#define MQTT_BROKER_ADDRESS "tcp://%s:%u"
#define CLIENT_ID_PREFIX    "MQTT_NC_"
#define CLIENT_ID_PREFIX_LISTENING    "MQTT_NC_L_"
#define DEFAULT_TOPIC   "nc_channel_pub"
#define MQTTCLIENT_TIMEOUT  1000
#define BUFFER_SIZE (1024 * 500)

// Global variables
application_options_t application_options;
int listen_status = -1;
int received_packets = 0;
int total_packets = 0;
int first_packet = 0;
int count = 0;
int n_packet = 0;
int delivered_packets = 0;
int need_clear = 0;
int wait_for_clear = 1;

/*
 * print_version - Function to print usage instructions
 * @cmd: command name
 *
 * This function Function to print usage instructions.
 */
static void print_usage(const char *cmd)
{
	printf("Usage: %s [<options>]\n", cmd);
	printf(" -h : show this help message\n");
	printf(" -v : show version\n");
	printf(" -T : topic for sharing messages: default = nc_channel_pub\n");
	printf(" -l : enable listening to incoming messages\n");
	printf(" -S : set server host : default = test.mosquitto.org\n");
	printf(" -P : set server port : default = 1883\n");
	printf(" -A : set server auth = user,pwd\n");
	printf("\n");
	printf("NCMQTT - Copyright (C) 2023 \n"
		   "This program comes with ABSOLUTELY NO WARRANTY!\n"
		   "This is free software licensed under GPL v3, and you are welcome to redistribute it under the conditions specified by this license.\n");
}

/*
 * print_version - Function to print version infomation
 *
 * This function Function to print version information.
 */
static void print_version(void)
{
	printf("ncmqtt %s (%s)\n", GIT_TAG, GIT_COMMIT);
	exit(0);
}

/*
 * print_progress_bar - Function to print progress bar
 * @total: total number of units to complete
 * @progress: number of units already completed
 * @mode: flag indicating sending or receiving mode
 *
 * This function prints a progress bar on stderr.
 */
static void print_progress_bar(int total, int progress, short mode)
{
	const int bar_width = 50; // Width of the progress bar
	float percentage = (total != 0) ? ((float)progress / total) * 100 : 0; // Calculate completion percentage

	// Calculate number of characters representing progress in the progress bar
	int progress_chars = (int)(percentage / (100.0 / bar_width));

	// Print the progress bar
	fprintf(stderr, "[");
	for (int i = 0; i < bar_width; i++) {
		if (i < progress_chars) {
			fprintf(stderr, "="); // Print "=" for completed part of the progress
		} else {
			fprintf(stderr, " "); // Print space for incomplete part of the progress
		}
	}

	// Print completion percentage and mode (sent/received)
	fprintf(stderr, "] %.2f%% (%s)\r", percentage, mode == 0 ? "sent" : "received");

	// Flush the stderr buffer to ensure immediate output
	fflush(stderr);
}

/*
 * generate_random_client_id - Generate a random MQTT client ID
 * @return: pointer to the generated client ID string
 *
 * This function generates a random MQTT client ID based on the application
 * options. If the application is set to listen for incoming messages,
 * the client ID will have a prefix indicating it is a listening client.
 */
static char *generate_random_client_id()
{
	char *client_id = (char *)malloc(sizeof(char) * 20);
	if (client_id) {
		if (application_options.listening) {
			snprintf(client_id, 20, "%s%d", CLIENT_ID_PREFIX_LISTENING, rand());
		}
		else {
			snprintf(client_id, 20, "%s%d", CLIENT_ID_PREFIX, rand());
		}
	}

	return client_id;
}

/*
 * on_delivery_complete - Callback function for delivery complete
 * @context: delivery context
 * @token: delivery token
 *
 * This function serves as a callback for delivery complete event
 * in MQTT message delivery.
 */
static void on_delivery_complete(void *context, MQTTClient_deliveryToken token)
{
	delivered_packets++;
}

/*
 * on_message - Callback function for handling received messages
 * @context: context for the callback
 * @topicName: name of the topic the message was received on
 * @topicLen: length of the topic name
 * @message: pointer to the received MQTT message
 * @return: integer indicating the success of the message handling
 *
 * This function is called when an MQTT message is received. It handles
 * the received messages, updating counters, printing progress bars,
 * and writing data to stdout.
 */
static int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	int ret = 1;

	if (application_options.listening) {
		// Handle received message
		if (message->payloadlen == 0) {
			wait_for_clear = 0;
			goto exit;
		}

		need_clear = 1;
		if ((message->payloadlen == 1) && (((char *)message->payload)[0] == '\4')) {
			//End file
			listen_status = 0;
			fprintf(stderr, "\n");
			goto exit;
		}

		if (first_packet == 0) {
			int *intptr = (int *)message->payload;
			total_packets = *intptr;
			first_packet = 1;
			goto exit;
		}

		received_packets++;
		print_progress_bar(total_packets, received_packets, 1);

		size_t nbyte = fwrite(message->payload, message->payloadlen, 1, stdout);

		goto exit;
	}

	if (message->payloadlen == 0) {
		wait_for_clear = 0;
	}

exit:
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);

	return ret;
}

int main(int argc, char *argv[])
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	int rc;
	int optres, errflg = 0;
	char server_host[256];
	struct stat file_stat;

	application_options.listening = false;
	application_options.topic = DEFAULT_TOPIC;
	application_options.server_host = "test.mosquitto.org";
	application_options.server_port = 1883;
	application_options.server_user = "";
	application_options.server_pwd = "";

	/******************************************/
	/*   Parse command line options...        */
	/******************************************/
	while ((optres = getopt(argc, argv, ":hvlT:S:P:A:")) != -1) {
		switch (optres) {
		case 'h':
			print_usage(argv[0]);
			return 0;

		case 'v':
			print_version();
			return 0;

		case 'l':
			application_options.listening = true;
			break;

		case 'T':
			application_options.topic = optarg;
			break;

		case 'S':
			application_options.server_host = optarg;
			break;

		case 'P':
			application_options.server_port = atoi(optarg);
			break;

		case 'A':
			application_options.server_user = strtok(optarg, ",");
			application_options.server_pwd = strtok(NULL, "\0");;
			break;

		case ':': /* -K without operand */
			fprintf(stderr, "Option -%c requires an operand\n\n", optopt);
			errflg++;
			break;

		case '?':
			fprintf(stderr, "Unrecognized option: -%c\n\n", optopt);
			errflg++;
			break;

		default:
			fprintf(stderr, "Unknown error while parsing command line options.\n\n");
			errflg++;
			break;
		}
	}

	if (errflg) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	char *random_client_id = generate_random_client_id();
	sprintf(server_host, MQTT_BROKER_ADDRESS, application_options.server_host, application_options.server_port);
	MQTTClient_create(&client, server_host, random_client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = application_options.server_user;
	conn_opts.password = application_options.server_pwd;

	MQTTClient_setCallbacks(client, NULL, NULL, on_message, on_delivery_complete);

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
		return -1;
	}

	if ((rc = MQTTClient_subscribe(client, application_options.topic, 0)) != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Failed to subscribe to MQTT broker, return code %d\n", rc);
		return 1;
	}

	if (application_options.listening) {
		while (listen_status != 0) {
			MQTTClient_yield();
			if (need_clear) {
				pubmsg.payload = NULL;
				pubmsg.payloadlen = 0;
				pubmsg.qos = 0;
				pubmsg.retained = 1;

				MQTTClient_publishMessage(client, application_options.topic, &pubmsg, &token);
				rc = MQTTClient_waitForCompletion(client, token, MQTTCLIENT_TIMEOUT);
				need_clear = 0;
			}
		}
		MQTTClient_yield();
		MQTTClient_unsubscribe(client, application_options.topic);
		MQTTClient_disconnect(client, 10000);
		MQTTClient_destroy(&client);
		return 0;
	}

	int ret_fstat = fstat(fileno(stdin), &file_stat);
	if (ret_fstat != 0) {
		perror("Error in fstat");
		return -1;
	}

	if (file_stat.st_size == 0) {
		fprintf(stderr, "Empty file\n");
		return -1;
	}

	n_packet = (int)ceil(file_stat.st_size / (float)BUFFER_SIZE);
	fprintf(stderr, "n_packet:%d\n", n_packet);

	pubmsg.payload = (void *)(&n_packet);
	pubmsg.payloadlen = (int)(sizeof(n_packet));
	pubmsg.qos = 0;
	pubmsg.retained = 1;

	MQTTClient_publishMessage(client, application_options.topic, &pubmsg, &token);
	rc = MQTTClient_waitForCompletion(client, token, MQTTCLIENT_TIMEOUT);

	count = 0;
	while (!feof(stdin)) {

		if (wait_for_clear) {
			usleep(100);
			continue;
		}

		char buffer[BUFFER_SIZE];
		size_t nbyte = fread(buffer, 1, BUFFER_SIZE, stdin);
		if (nbyte == 0) {
			return -1;
		}

		pubmsg.payload = (void *)buffer;
		pubmsg.payloadlen = (int)nbyte;
		pubmsg.qos = 0;
		pubmsg.retained = 1;

		MQTTClient_publishMessage(client, application_options.topic, &pubmsg, &token);
		rc = MQTTClient_waitForCompletion(client, token, MQTTCLIENT_TIMEOUT);

		wait_for_clear = 1;
		count++;
		print_progress_bar(n_packet, count, 0);
	}

	fprintf(stderr, "\n");

	pubmsg.payload = "\4";
	pubmsg.payloadlen = 1;
	pubmsg.qos = 0;
	pubmsg.retained = 1;

	MQTTClient_publishMessage(client, application_options.topic, &pubmsg, &token);
	rc = MQTTClient_waitForCompletion(client, token, MQTTCLIENT_TIMEOUT);

	MQTTClient_unsubscribe(client, application_options.topic);

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);

	return 0;
}

