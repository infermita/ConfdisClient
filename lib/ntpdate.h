#ifndef NTPDATE_H
#define NTPDATE_H

#define NTP_TIMESTAMP_DELTA 2208988800ull

class NtpDate
{
public:
    NtpDate();
    static bool SetDate(char *ip);
};

#endif // NTPDATE_H
