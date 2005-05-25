/*
 * Parabellym: an environment for building extensible modular applications.
 * Copyright (c) 2003-2004 The Faerion Team, http://www.faerion.oss/
 * Read the LICENSE file for details about the distribution.
 *
 * $Id$
 *
 */

#ifndef __parabellum_api_h
#define __parabellum_api_h

/* We need this for size_t */
#include <stddef.h>

/* We need this for bool in C99 mode */
#include <stdbool.h>

/* We need strlen to assign strings to para_iov_t */
#if defined(__cplusplus)
# include <string.h>
#endif

enum para_error_codes
{
	/* message queue */
	PEC_MSGQ_BAD_ID = -400,
	PEC_MSGQ_EMPTY,
	PEC_MSGQ_ERROR,

	/* messages */
	PEC_MSG_NOT_FOUND = -300,
	PEC_MSG_BAD_ID,
	PEC_MSG_FAILED,
	PEC_MSG_NO_RECIPIENT,

	/* modules */
	PEC_MOD_FAILED = -200,
	PER_MOD_INVALID,
	PEC_MOD_NOT_LOADED,
	PEC_MOD_ENTRY_MISSING,
	PEC_MOD_REQUIRED,
	PEC_MOD_BAD_ID,

	/* unsorted */
	PEC_INVALID_ARGUMENT = -100,
	PEC_MEMORY_ERROR,
	PEC_INIT_FAILURE,
	PEC_HAVE_MODULES,
	PEC_NOT_IMPLEMENTED,
	PEC_UNAUTHORIZED,
	PEC_SIGNAL,

	/* no error */
	PEC_SUCCESS = 0,
};

enum para_unload_mode
{
	PUC_NORMAL,
	PUC_WAIT,
	PUC_EMERGENCY,
};

/* system signals */
#define PSIG_UNLOAD -1

#ifdef _WIN32
# define __pc_export __declspec(dllexport)
# define __pc_import __declspec(dllimport)
#else
# define __stdcall
# define __pc_export
# define __pc_import
#endif

#ifdef __cplusplus
# define __pc_ext extern "C"
#else
# define __pc_ext
#endif

#ifdef PARA_BUILD_CORE
# define PARA_API(type) __pc_ext type __stdcall __pc_export
#else
# define PARA_API(type) __pc_ext type __stdcall __pc_import
__pc_ext void __stdcall __pc_export para_mod_body(int qid);
#endif

typedef struct para_iov_t
{
	const void *data;
	size_t size;
#if defined(__cplusplus)
	para_iov_t(void)
	{
		data = (void*)0;
		size = 0;
	}
	para_iov_t(void *_data, size_t _size)
	{
		data = _data;
		size = _size;
	}
	operator const para_iov_t * (void)
	{
		return this;
	}
	const char * as_str() const
	{
		return reinterpret_cast<const char *>(data);
	}
	int as_int() const
	{
		return * reinterpret_cast<const int *>(data);
	}
	void * as_ptr() const
	{
		return const_cast<void *>(* reinterpret_cast<void * const *>(data));
	}
	para_iov_t& operator = (const char *nts)
	{
		data = nts;
		size = nts ? strlen(nts) + 1 : 0;
		return *this;
	}
#endif /* __cplusplus */
} para_iov_t;

typedef struct para_msgi_s para_msgi_t;

/* Initialization and shutdown */
PARA_API(int) para_core_init(void);
PARA_API(int) para_core_shutdown(enum para_unload_mode);
/* Module management */
PARA_API(int) para_mod_load(const char *modname);
/* Message passing */
PARA_API(int) para_msg_attach(int msgq);
PARA_API(int) para_msg_find(const char *msgname);
PARA_API(int) para_msg_receive(para_msgi_t *);
PARA_API(int) para_msg_reply(const para_msgi_t *);
PARA_API(int) para_msg_send(para_msgi_t *);
PARA_API(int) para_msg_signal(int modid, int sig);
PARA_API(int) para_msg_subscribe(const char *name);
PARA_API(int) para_msg_unsubscribe(int msgid);
/* Error messsages */
PARA_API(const char *) para_errstr(int);

struct para_msgi_s
{
	int msgid;
	unsigned int attc;
	para_iov_t *attv;
	bool rreq;
	int signal;
#if defined(__cplusplus)
	para_msgi_s(int msgid = 0, unsigned int attc = 0, para_iov_t *attv = NULL, bool rreq = false, int signal = 0)
	{
		this->msgid = msgid;
		this->attc = attc;
		this->attv = attv;
		this->rreq = rreq;
		this->signal = signal;
	}
	bool operator == (int msgid)
	{
		return (this->msgid == msgid);
	}
	bool operator != (int msgid)
	{
		return (this->msgid != msgid);
	}
	int receive(void)
	{
		return para_msg_receive(this);
	}
	int reply(void) const
	{
		return para_msg_reply(this);
	}
	int send(void)
	{
		return para_msg_send(this);
	}
#endif
};

#if defined(__cplusplus)
namespace parabellym
{
	class msgid
	{
		int _msgid;
	public:
		operator int () const
		{
			return _msgid;
		}
		bool is_valid() const
		{
			return (_msgid > 0);
		}
		msgid(const char *name)
		{
			_msgid = para_msg_subscribe(name);
		}
		~msgid()
		{
			if (is_valid())
				para_msg_unsubscribe(_msgid);
		}
	};
};
#endif

#endif /* __parabellum_api_h */
