// Parabellym: an environment for building extensible modular applications.
// Copyright (c) 2003-2005 The Faerion Team, http://www.faerion.oss/
// Read the LICENSE file for details about the distribution.
//
// $Id$

#ifndef __parabellym_plus_h
#define __parabellym_plus_h

#include "api.h"
#include <string.h>
#include <wchar.h>
#include <map>
#include <string>
#include <vector>

namespace parabellym
{
	class exception
	{
		int mCode;
		std::string mText;
	public:
		exception(int id)
		{
			mCode = id;
			mText = std::string(para_errstr(mCode));
		}
		exception(const std::string &text)
		{
			mCode = 0;
			mText = text;
		}
		const std::string& as_string() const { return mText; }
		const char * as_cstring() const { return mText.c_str(); }
	};

	class attachment : public para_iov_t
	{
	public:
		attachment(void *d = 0, size_t s = 0)
		{
			data = d;
			size = s;
		}
		attachment(const attachment &src)
		{
			data = src.data;
			size = src.size;
		}
		attachment(const para_iov_t &src)
		{
			data = src.data;
			size = src.size;
		}
		attachment(const char *s)
		{
			data = s;
			size = s ? strlen(s) + 1 : 0;
		}
		attachment(const wchar_t *s)
		{
			data = s;
			size = s ? (wcslen(s) + 1) * sizeof(wchar_t) : 0;
		}
		attachment(const std::string &s)
		{
			data = s.c_str();
			size = s.size() + 1;
		}
		attachment(const std::wstring &s)
		{
			data = s.c_str();
			size = (s.size() + 1) * sizeof(wchar_t);
		}
		template <class T>
		attachment(const T &s)
		{
			data = &s;
			size = sizeof(*&s);
		}
		operator const para_iov_t * ()
		{
			return this;
		}
		const char * as_cstring() const
		{
			return reinterpret_cast<const char *>(data);
		}
		std::string as_string() const
		{
			return std::string(as_cstring());
		}
		int as_integer() const
		{
			return * reinterpret_cast<const int *>(data);
		}
		void * as_pointer() const
		{
			return const_cast<void *>(* reinterpret_cast<void * const *>(data));
		}
	};

	typedef std::vector<attachment> attachments;

	class module_ne
	{
		int mQueueId;
		int mLastErr;
		std::map<int, std::string> mFwdMap;
		std::map<std::string, int> mRevMap;
	protected:
		bool mRunLoop;
		int get_queue_id() const { return mQueueId; }
		// Last occured error.
		int get_last_error_id() const { return mLastErr; }
		std::string get_last_error() const { return std::string(para_errstr(mLastErr)); }
		// Test a return code for success.
		static bool isok(int id) { return (id >= PEC_SUCCESS); }
		// Subscription management.
		bool subscribe(const std::string &name)
		{
			if (!isok(mLastErr = para_msg_subscribe(name.c_str())))
				return false;
			mFwdMap[mLastErr] = name;
			mRevMap[name] = mLastErr;
			return true;
		}
		bool unsubscribe(int msgid)
		{
			if (!isok(mLastErr = para_msg_unsubscribe(msgid)))
				return false;
			mRevMap.erase(mRevMap.find(mFwdMap[msgid]));
			mFwdMap.erase(mFwdMap.find(msgid));
			return true;
		}
		bool unsubscribe(const std::string &name)
		{
			std::map<std::string, int>::const_iterator it = mRevMap.find(name);
			if (it == mRevMap.end())
				return false;
			return unsubscribe(it->second);
		}
		// Sending messages without requiring a response.
		bool send(int msgid, const attachments &att)
		{
			para_msgi_t mi;
			
			mi.msgid = msgid;
			mi.attc = att.size();
			mi.attv = &att[0];
			mi.rreq = false;
			mi.signal = 0;

			return isok(mLastErr = para_msg_send(&mi));
		}
		bool send(const std::string &name, const attachments &att)
		{
			std::map<std::string, int>::const_iterator it = mRevMap.find(name);
			if (it == mRevMap.end())
				return false;
			return send(it->second, att);
		}

		// Sending messages requiring a response.
		bool send(int msgid, const attachments &att, attachments &resp)
		{
			para_msgi_t mi;

			mi.msgid = msgid;
			mi.attc = att.size();
			mi.attv = &att[0];
			mi.rreq = false;
			mi.signal = 0;

			if (isok(mLastErr = para_msg_send(&mi))) {
				// clear() presumably deallocates all memory, purging the
				// unused cached entries, killing the performance.
				resp.resize(0);

				for (unsigned int idx = 0; idx < mi.attc; ++idx)
					resp.push_back(attachment(mi.attv[idx]));

				return true;
			}

			return false;
		}

		bool send(const std::string &name, const attachments &att, attachments &resp)
		{
			std::map<std::string, int>::const_iterator it = mRevMap.find(name);
			if (it == mRevMap.end())
				return false;
			return send(it->second, att, resp);
		}
		// Posting a signal.
		bool signal(int sigid, int qid = 0)
		{
			if (qid == 0)
				qid = mQueueId;
			return isok(mLastErr = para_msg_signal(qid, sigid));
		}
		// Replying to a message.
		bool reply(const attachments &att)
		{
			para_msgi_t mi;
			mi.attc = att.size();
			mi.attv = &att[0];
			return isok(mLastErr = para_msg_reply(&mi));
		}
		// Handling messages.
		virtual void on_message(int /*msgid*/, const std::string & /*msgname*/, const attachments & /*att*/, bool /*needs_answer*/) { }
		virtual void on_signal(int /*sigid*/) { }
		// Exit module.
		void exit() { mRunLoop = false; }
	public:
		module_ne(int QueueId)
		{
			mQueueId = QueueId;
			mLastErr = PEC_SUCCESS;
		}
		virtual ~module_ne() { }
		virtual void run()
		{
			para_msgi_t mi;

			mRunLoop = true;

			while (mRunLoop) {
				switch (mLastErr = para_msg_receive(&mi)) {
				case PEC_SUCCESS:
					{
						std::map<int, std::string>::const_iterator it = mFwdMap.find(mi.msgid);
						if (it != mFwdMap.end()) {
							attachments att;
							for (unsigned int idx = 0; idx < mi.attc; ++idx)
								att.push_back(mi.attv[idx]);
							on_message(mi.msgid, it->second, att, mi.rreq);
						}
					}
					break;
				case PEC_SIGNAL:
					if (mi.signal == PSIG_UNLOAD)
						mRunLoop = false;
					else
						on_signal(mi.signal);
					break;
				default:
					mRunLoop = false;
				}
			}
		}
	};

	class module : module_ne
	{
	public:
		module(int QueueId) : module_ne(QueueId) { }
		virtual ~module() { }
		int get_last_error_id() const { return module_ne::get_last_error_id(); }
		int subscribe(const std::string &name)
		{
			if (!module_ne::subscribe(name))
				throw exception(get_last_error_id());
			return get_last_error_id();
		}
		void unsubscribe(int msgid)
		{
			if (!module_ne::unsubscribe(msgid))
				throw exception(get_last_error_id());
		}
		void unsubscribe(const std::string &name)
		{
			if (!module_ne::unsubscribe(name))
				throw exception(get_last_error_id());
		}
		void send(int msgid, const attachments &att)
		{
			if (!module_ne::send(msgid, att))
				throw exception(get_last_error_id());
		}
		void send(const std::string &name, const attachments &att)
		{
			if (!module_ne::send(name, att))
				throw exception(get_last_error_id());
		}
		void send(int msgid, const attachments &att, attachments &resp)
		{
			if (!module_ne::send(msgid, att, resp))
				throw exception(get_last_error_id());
		}
		void send(const std::string &name, const attachments &att, attachments &resp)
		{
			if (!module_ne::send(name, att, resp))
				throw exception(get_last_error_id());
		}
		void signal(int sigid, int qid = 0)
		{
			if (!module_ne::signal(sigid, qid))
				throw exception(get_last_error_id());
		}
		void reply(const attachments &att)
		{
			if (!module_ne::reply(att))
				throw exception(get_last_error_id());
		}
		void exit() { module_ne::exit(); }
		virtual void on_message(int /*msgid*/, const std::string & /*msgname*/, const attachments & /*att*/, bool /*needs_answer*/) { }
		virtual void on_signal(int /*sigid*/) { }
		virtual void run() { module_ne::run(); }
	};
};

#endif
