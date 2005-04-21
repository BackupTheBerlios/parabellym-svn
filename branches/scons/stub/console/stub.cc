// Copyright (c) 2003 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// This file shows how a Parabellym loader could be implemented.
// Thought this is only an example, it is a fully functional
// loader.  It supports loading of additional modules from the
// command line.
//
// If after the modules are loaded there is no "parabellym.rock-and-roll"
// message handlers, the modules are unloaded and the stub shuts down.
// Otherwise an empty message is sent.  The module that receives the message
// must do the work it is designed for, such as entering the event loop
// if that is a server application.  When the message handler returns, the
// system is shut down.
//
// $Id$

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <unistd.h>
#endif
#ifdef HAVE_getopt
# include <getopt.h>
#endif
#include "stub.h"

static const char *usage =
	"Usage: parastub [OPTION]... [- [MODULE]...]\n"
	"\n"
	"Options:\n"
	"  -d   : daemonize (detach from console)\n"
	"\n"
	"Report bugs to <faerion-devel@lists.sourceforge.net>\n"
	;

#ifdef _WIN32
static BOOL WINAPI console_ctrl_handler(DWORD event)
{
	switch (event) {
	case CTRL_BREAK_EVENT:
		printf("Detaching from console.\n");
		FreeConsole();
		return 1;
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		fprintf(stdout, "Terminating...");
		return 1;
	default:
		return 0;
	}
}
#endif

const char *get_file_name(const char *fname)
{
	const char *src = fname;

	while (*fname != '\0')
		++fname;

	while (fname != src && *fname != '/' && *fname != '\\')
		--fname;

	if (*fname == '/' || *fname == '\\')
		++fname;

	return fname;
}

static void sigint(int sig)
{
	fprintf(stdout, "\nEmergency shutdown.\n");
	signal(sig, sigint);
	para_core_shutdown(PUC_EMERGENCY);
}

int main(int argc, char * const * argv)
{
	char ch;
	int rc, modcount = 0;
	bool detach = false;

	while ((ch = getopt(argc, argv, "d")) != -1) {
		switch (ch) {
		case 'd':
			detach = true;
			break;
		default:
			fprintf(stdout, "%s", usage);
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	fprintf(stdout, "Parabellym loader v%s (use the -? switch for usage info).\n", VERSION);

	if (para_core_init() != 0) {
		fprintf(stderr, "Could not initialize the parabellym core.\n");
		return 1;
	}

	fprintf(stdout, "Parabellym core loaded ok.\n");

	if (detach) {
		fprintf(stdout, "Detaching from console.\n");
		daemon(1, 0);
	}

	while (argc != 0) {
		fprintf(stdout, "Loading module %s...", argv[0]);
		if (para_mod_load(argv[0]) == PEC_SUCCESS) {
			fprintf(stdout, " done.\n");
			++modcount;
		} else {
			fprintf(stdout, " failed.\n");
		}
		--argc, ++argv;
	}

	if (modcount == 0) {
		fprintf(stdout, "No modules loaded, exiting.\n");
		return 1;
	}

#ifdef _WIN32
	if (detach)
		FreeConsole();
	else {
		char title[1024];

		if (SetConsoleCtrlHandler(console_ctrl_handler, TRUE)) {
			fprintf(stdout, "Press Ctrl+Break to detach from console.\n");
			fprintf(stdout, "Press Ctrl+C to interrupt the program.\n");
		}

		snprintf(title, sizeof(title), "Parabellym, v%u.%u.%u.%u", VERSION_NUM);
		SetConsoleTitle(title);
	}
#else
	signal(SIGINT, sigint);
#endif

	fprintf(stdout, "Waiting for all modules to unload.\n");
	if ((rc = para_core_shutdown(PUC_WAIT)) != PEC_SUCCESS)
		fprintf(stdout, "  could not shut down properly: %s\n", para_errstr(rc));

	fprintf(stdout, "Over and out.\n");
	return 0;
}
