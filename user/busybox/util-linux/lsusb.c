/* vi: set sw=4 ts=4: */
/*
 * lsusb implementation for busybox
 *
 * Copyright (C) 2009  Malek Degachi <malek-degachi@laposte.net>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

//usage:#define lsusb_trivial_usage NOUSAGE_STR
//usage:#define lsusb_full_usage ""

#include "libbb.h"

static int FAST_FUNC fileAction(
		const char *fileName,
		struct stat *statbuf UNUSED_PARAM,
		void *userData UNUSED_PARAM,
		int depth UNUSED_PARAM)
{
	parser_t *parser;
	char *tokens[6];
	char *bus = NULL, *device = NULL;
	int product_vid = 0, product_did = 0;

	char *uevent_filename = concat_path_file(fileName, "/uevent");
	parser = config_open2(uevent_filename, fopen_for_read);
	free(uevent_filename);

	while (config_read(parser, tokens, 6, 1, "\\/=", PARSE_NORMAL)) {
		if (strcmp(tokens[0], "DRIVER") == 0) {
			if (strcmp(tokens[1], "usb") != 0)
				break;
		}

		if (strcmp(tokens[0], "DEVICE") == 0) {
			bus = xstrdup(tokens[4]);
			device = xstrdup(tokens[5]);
			continue;
		}

		if (strcmp(tokens[0], "PRODUCT") == 0) {
			product_vid = xstrtou(tokens[1], 16);
			product_did = xstrtou(tokens[2], 16);
			continue;
		}
	}
	config_close(parser);

	if (bus) {
		printf("Bus %s Device %s: ID %04x:%04x\n", bus, device, product_vid, product_did);
		free(bus);
		free(device);
	}

	return TRUE;
}

int lsusb_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int lsusb_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	/* no options, no getopt */

	recursive_action("/sys/bus/usb/devices",
			ACTION_RECURSE,
			fileAction,
			NULL, /* dirAction */
			NULL, /* userData */
			0 /* depth */);

	return EXIT_SUCCESS;
}
