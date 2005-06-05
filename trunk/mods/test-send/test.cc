// parafile: simple profile storage for Parabellym
// Copyright (C) 2003 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// The receiving part.

#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
# include <windows.h>
# define sleep(x) Sleep(x * 1000)
#else
# include <unistd.h>
#endif
#include "../../core/plus.h"

using namespace parabellym;

class sendmodule : public module
{
	std::string mMessage;
public:
	sendmodule(int qid) : module(qid)
	{
		mMessage = "oss.faerion.parabellym.test";
	}

	~sendmodule()
	{
	}

	// The standard run() method waits for incoming messages.
	// We need a different behaviour, so we override it.
	void run()
	{
		attachments att;
		attachments rep;

		subscribe(mMessage);

		for (int idx = 10; idx > 0; --idx) {
			try {
				send(mMessage, att, rep);
				printf(" S > message sent, response contains %u data blocks [%08X].\n", (unsigned)rep.size(), get_last_error_id());
				break;
			} catch (exception &e) {
				printf(" S > exception: %s, retrying.\n", e.as_cstring());
				sleep(1);
			}
		}
	}
};

para_module()
{
	sleep(1);

	try {
		sendmodule mod(QueueId);
		printf(" S : starting.\n");
		mod.run();
	} catch (exception &e) {
		printf(" S ! exception: %s.\n", e.as_cstring());
	}
}
