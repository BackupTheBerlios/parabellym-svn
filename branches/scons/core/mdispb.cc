// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$

#include <string.h>
#include <faeutil/sofunc.h>
#include "debug.h"
#include "mdisp.h"
#include "message.h"

faeutil::stock<mdispb *> mdispb::stock;

faeutil::sem mdispb::mrsem;

mdispb::mdispb(para_modbody_fn f, void *h)
{
	blocked = NULL;
	func = f;
	handle = h;
	queue.create();
	id = stock.add(this);
}

mdispb::~mdispb()
{
	stock.remove(id);
	mrsem.post();
}

void* mdispb::bodyw(void *arg)
{
	mdispb *md = reinterpret_cast<mdispb *>(arg);
	md->body();
	os_freemod(md->handle);
	delete md;
	return NULL;
}

void mdispb::body()
{
	tlsk.set(this);

	log((flog, flMod, "%x: entering the body", this));
	func(queue.get_id());
	log((flog, flMod, "%x: left the body", this));
}

void mdispb::wait_unload()
{
	while (stock.size() != 0) {
		mrsem.wait();
	}
}
