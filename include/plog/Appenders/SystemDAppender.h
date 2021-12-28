#pragma once
#include <plog/Appenders/IAppender.h>
#include <systemd/sd-journal.h>

namespace plog
{
    template <class Formatter>
    class SystemDAppender : public IAppender
    {
    private:
        static int logSeverityToType(Severity severity)
        {
            switch (severity)
            {
            case none:
                return LOG_ALERT;
            case fatal:
                return LOG_CRIT;
            case error:
                return LOG_ERR;
            case warning:
                return LOG_WARNING;
            case info:
                return LOG_NOTICE;
            case debug:
                return LOG_INFO;
            case verbose:
            default:
                return LOG_DEBUG;
            }
        }
    public:
        virtual void write(const Record& record)
        {
            util::nstring str = Formatter::format(record);
            int prio = logSeverityToType(record.getSeverity());
            // Could use sd_journal_send_with_location, if record has good location
#ifdef NDEBUG
            int r=sd_journal_send(
#else // !NDEBUG
            int r=sd_journal_send_with_location(
                record.getFile(), std::to_string(record.getLine()).c_str(), record.getFunc(),
#endif // !NDEBUG
                "MESSAGE=%s", str.c_str(),
                "PRIORITY=%i", prio,
                "TID=%u", record.getTid(),
                NULL);
            if (r<0) {
                // fprintf(stderr, "sd_journal_print error: %s\n", strerror(-r));
            }
        }
    };
}
