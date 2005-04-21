// Copyright (c) 2003-2005 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$

#ifndef __parabellum_core_msgp_h
#define __parabellum_core_msgp_h

class para_iov_t;
class mdisp;

// The message packet class.  The main function is to convert
// a message into a contiguous, position-independent block of
// data available for cloning and transmitting over the netowrk.
// Instances of this class can be copied -- they will all refer
// to the same packet.  When deleted, the data remains floating
// in the memory; make sure to call the `detach' method when
// an instance is no longer needed.
class msgp
{
	// The serialized message.  The layout is:
	// - magic number (32bit),
	// - packet size (32bit),
	// - message code (32bit),
	// - the number of attachments (32bit),
	// - the attached data.
	void *data;
public:
	// Builds an empty packet.
	msgp();
	// Builds a packet from a mesasge.  The module id is used
	// to deliver a response.
	msgp(const para_msgi_t *info, mdisp* blocked);
	// Nothing.
	~msgp() { }
	// Copies the reference.
	msgp& operator = (const msgp &src) { data = src.data; return *this; }
	// Returns the reference.
	operator void * () const { return data; }
	// Deserializes the packet.
	void map(para_msgi_t *info, mdisp *& blocked);
	// Copies the packet.
	void clone(const msgp &src);
	// Detaches the data from the object.
	void detach() { data = NULL; }
	// Erases the data (doesn't give a heck about references).
	void free();
	// Checks whether the packet is empty.
	bool is_empty() const { return data == NULL; }
};

#endif // __parabellum_core_msgp_h
