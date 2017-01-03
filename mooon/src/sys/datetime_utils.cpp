/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: eyjian@qq.com or eyjian@gmail.com
 */
#include "sys/datetime_utils.h"
#include <pthread.h> // localtime_r
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <utils/string_utils.h>
SYS_NAMESPACE_BEGIN

bool CDatetimeUtils::is_same_day(time_t t1, time_t t2)
{
    struct tm result1;
    struct tm result2;

    localtime_r(&t1, &result1);
    localtime_r(&t2, &result2);

    return (result1.tm_year == result2.tm_year) &&
           (result1.tm_mon == result2.tm_mon) &&
           (result1.tm_mday == result2.tm_mday);
}

bool CDatetimeUtils::is_leap_year(int year)
{
    return ((0 == year%4) && (year%100 != 0)) || (0 == year%400);
}

uint32_t CDatetimeUtils::time2date(time_t t)
{
    struct tm result;
    localtime_r(&t, &result);

    // 20160114
    return (result.tm_year+1900)*10000 + (result.tm_mon+1)*100 + result.tm_mday;
}

bool CDatetimeUtils::neighbor_date_bytime(const std::string& datetime, int days, std::string* neighbor_date)
{
    time_t t;
    if (!datetime_struct_from_string(datetime.c_str(), &t))
        return false;

    time_t z = t + (days * 24 * 3600);
    *neighbor_date = to_date(z);
    return true;
}

std::string CDatetimeUtils::neighbor_date_bytime(const std::string& datetime, int days)
{
    std::string neighbor_date;
    (void)neighbor_date_bytime(datetime, days, &neighbor_date);
    return neighbor_date;
}

bool CDatetimeUtils::neighbor_date_bydate(const std::string& date, int days, std::string* neighbor_date)
{
    const std::string datetime = date + std::string(" 00:00:00");
    return neighbor_date_bytime(datetime, days, neighbor_date);
}

std::string CDatetimeUtils::neighbor_date_bydate(const std::string& date, int days)
{
    std::string neighbor_date;
    (void)neighbor_date_bydate(date, days, &neighbor_date);
    return neighbor_date;
}

std::string CDatetimeUtils::extract_date(const char* datetime)
{
    const char* s = strchr(datetime, ' ');
    return (NULL == s)? std::string(""): std::string(datetime, s-datetime);
}

std::string CDatetimeUtils::extract_date(const std::string& datetime)
{
    std::string::size_type pos = datetime.find(' ');
    if (pos == std::string::npos)
    {
        return std::string("");
    }
    else
    {
        return datetime.substr(0, pos);
    }
}

std::string CDatetimeUtils::extract_time(const char* datetime)
{
    const char* s = strchr(datetime, ' ');
    if (NULL == s)
        return std::string("");

    return s + 1;
}

std::string CDatetimeUtils::extract_time(const std::string& datetime)
{
    std::string::size_type pos = datetime.find(' ');
    if (pos == std::string::npos)
    {
        return std::string("");
    }
    else
    {
        return datetime.substr(pos+1);
    }
}

void CDatetimeUtils::extract_datetime(const char* datetime, std::string* date, std::string* time)
{
    const char* s = strchr(datetime, ' ');
    if (s != NULL)
    {
        date->assign(datetime, s-datetime);
        *time = s + 1;
    }
}

void CDatetimeUtils::extract_datetime(const std::string& datetime, std::string* date, std::string* time)
{
    std::string::size_type pos = datetime.find(' ');
    if (pos != std::string::npos)
    {
        *date = datetime.substr(0, pos);
        *time = datetime.substr(pos + 1);
    }
}

void CDatetimeUtils::get_current_datetime(char* datetime_buffer, size_t datetime_buffer_size, const char* format)
{
    struct tm result;
    time_t now = time(NULL);

    localtime_r(&now, &result);
    snprintf(datetime_buffer, datetime_buffer_size
        ,format
        ,result.tm_year+1900, result.tm_mon+1, result.tm_mday
        ,result.tm_hour, result.tm_min, result.tm_sec);
}

std::string CDatetimeUtils::get_current_datetime(const char* format)
{
    char datetime_buffer[sizeof("YYYY-MM-DD HH:SS:MM")+100];
    get_current_datetime(datetime_buffer, sizeof(datetime_buffer), format);
    return datetime_buffer;
}

void CDatetimeUtils::get_current_date(char* date_buffer, size_t date_buffer_size, const char* format)
{
    struct tm result;
    time_t now = time(NULL);

    localtime_r(&now, &result);
    snprintf(date_buffer, date_buffer_size
        ,format
        ,result.tm_year+1900, result.tm_mon+1, result.tm_mday);
}

std::string CDatetimeUtils::get_current_date(const char* format)
{
    char date_buffer[sizeof("YYYY-MM-DD")+100];
    get_current_date(date_buffer, sizeof(date_buffer), format);
    return date_buffer;
}

void CDatetimeUtils::get_current_time(char* time_buffer, size_t time_buffer_size, const char* format)
{
    struct tm result;
    time_t now = time(NULL);

    localtime_r(&now, &result);
    snprintf(time_buffer, time_buffer_size
        ,format
        ,result.tm_hour, result.tm_min, result.tm_sec);
}

std::string CDatetimeUtils::get_current_time(const char* format)
{
    char time_buffer[sizeof("HH:SS:MM")+100];
    get_current_time(time_buffer, sizeof(time_buffer), format);
    return time_buffer;
}

void CDatetimeUtils::get_current_datetime_struct(struct tm* current_datetime_struct)
{
    time_t now = time(NULL);
    localtime_r(&now, current_datetime_struct);
}

void CDatetimeUtils::decompose(const struct tm* tm, int* year, int* month, int* day, int64_t* hour, int64_t* minute, int64_t* second)
{
    if (year != NULL)
    {
        *year = tm->tm_year+1900;
    }
    if (month != NULL)
    {
        *month = (tm->tm_year+1900)*100 + tm->tm_mon+1;
    }
    if (day != NULL)
    {
        *day = (tm->tm_year+1900)*10000 + (tm->tm_mon+1)*100 + tm->tm_mday;
    }
    if (hour != NULL)
    {
        *hour = (tm->tm_year+1900)*1000000LL + (tm->tm_mon+1)*10000 + tm->tm_mday*100 + tm->tm_hour;
    }
    if (minute != NULL)
    {
        *minute = (tm->tm_year+1900)*100000000LL + (tm->tm_mon+1)*1000000 + tm->tm_mday*10000 + tm->tm_hour*100 + tm->tm_min;
    }
    if (second != NULL)
    {
        *second = (tm->tm_year+1900)*10000000000LL + (tm->tm_mon+1)*100000000 + tm->tm_mday*1000000 + tm->tm_hour*10000 + tm->tm_min*100 + tm->tm_sec;
    }
}

void CDatetimeUtils::decompose(const struct tm* tm, std::string* year, std::string* month, std::string* day, std::string* hour, std::string* minute, std::string* second)
{
    int Y, M, D; // 年月日
    int64_t h, m, s; // 时分秒

    decompose(tm, &Y, &M, &D, &h, &h, &s);
    if (year != NULL)
        *year = utils::CStringUtils::int_tostring(Y);
    if (month != NULL)
        *month = utils::CStringUtils::int_tostring(M);
    if (day != NULL)
        *day = utils::CStringUtils::int_tostring(D);
    if (hour != NULL)
        *hour = utils::CStringUtils::int_tostring(h);
    if (minute != NULL)
        *minute = utils::CStringUtils::int_tostring(m);
    if (second != NULL)
        *second = utils::CStringUtils::int_tostring(s);
}

void CDatetimeUtils::decompose(const struct tm& tm, int* year, int* month, int* day, int64_t* hour, int64_t* minute, int64_t* second)
{
    decompose(&tm, year, month, day, hour, minute, second);
}

void CDatetimeUtils::decompose(const struct tm& tm, std::string* year, std::string* month, std::string* day, std::string* hour, std::string* minute, std::string* second)
{
    decompose(&tm, year, month, day, hour, minute, second);
}

void CDatetimeUtils::decompose(time_t t, int* year, int* month, int* day, int64_t* hour, int64_t* minute, int64_t* second)
{
    struct tm tm;
    localtime_r(&t, &tm);
    decompose(&tm, year, month, day, hour, minute, second);
}

void CDatetimeUtils::decompose(time_t t, std::string* year, std::string* month, std::string* day, std::string* hour, std::string* minute, std::string* second)
{
    struct tm tm;
    localtime_r(&t, &tm);
    decompose(&tm, year, month, day, hour, minute, second);
}

std::string CDatetimeUtils::to_str_long_year(const struct tm& t, const char* prefix)
{
    if (NULL == prefix)
        return utils::CStringUtils::format_string("%04d-01-01 00:00:00", t.tm_year+1900);
    else
        return utils::CStringUtils::format_string("%s%04d-01-01 00:00:00", prefix, t.tm_year+1900);
}

std::string CDatetimeUtils::to_str_long_month(const struct tm& t, const char* prefix)
{
    if (NULL == prefix)
        return utils::CStringUtils::format_string("%04d-%02d-01 00:00:00", t.tm_year+1900, t.tm_mon+1);
    else
        return utils::CStringUtils::format_string("%s%04d-%02d-01 00:00:00", prefix, t.tm_year+1900, t.tm_mon+1);
}

std::string CDatetimeUtils::to_str_long_day(const struct tm& t, const char* prefix)
{
    if (NULL == prefix)
        return utils::CStringUtils::format_string("%04d-%02d-%02d 00:00:00", t.tm_year+1900, t.tm_mon+1, t.tm_mday);
    else
        return utils::CStringUtils::format_string("%s%04d-%02d-%02d 00:00:00", prefix, t.tm_year+1900, t.tm_mon+1, t.tm_mday);
}

std::string CDatetimeUtils::to_str_long_hour(const struct tm& t, const char* prefix)
{
    if (NULL == prefix)
        return utils::CStringUtils::format_string("%04d-%02d-%02d %02d:00:00", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour);
    else
        return utils::CStringUtils::format_string("%s%04d-%02d-%02d %02d:00:00", prefix, t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour);
}

std::string CDatetimeUtils::to_str_long_minute(const struct tm& t, const char* prefix)
{
    if (NULL == prefix)
        return utils::CStringUtils::format_string("%04d-%02d-%02d %02d:%02d:00", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min);
    else
        return utils::CStringUtils::format_string("%s%04d-%02d-%02d %02d:%02d:00", prefix, t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min);
}

std::string CDatetimeUtils::to_str_long_second(const struct tm& t, const char* prefix)
{
    if (NULL == prefix)
        return utils::CStringUtils::format_string("%04d-%02d-%02d %02d:%02d:%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    else
        return utils::CStringUtils::format_string("%s%04d-%02d-%02d %02d:%02d:%02d", prefix, t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

void CDatetimeUtils::to_current_datetime(struct tm* current_datetime_struct, char* datetime_buffer, size_t datetime_buffer_size, const char* format)
{
    snprintf(datetime_buffer, datetime_buffer_size
        ,format
        ,current_datetime_struct->tm_year+1900, current_datetime_struct->tm_mon+1, current_datetime_struct->tm_mday
        ,current_datetime_struct->tm_hour, current_datetime_struct->tm_min, current_datetime_struct->tm_sec);
}

std::string CDatetimeUtils::to_current_datetime(struct tm* current_datetime_struct, const char* format)
{
    char datetime_buffer[sizeof("YYYY-MM-DD HH:SS:MM")+100];
    to_current_datetime(current_datetime_struct, datetime_buffer, sizeof(datetime_buffer), format);
    return datetime_buffer;
}

void CDatetimeUtils::to_current_date(struct tm* current_datetime_struct, char* date_buffer, size_t date_buffer_size, const char* format)
{
    snprintf(date_buffer, date_buffer_size
        ,format
        ,current_datetime_struct->tm_year+1900, current_datetime_struct->tm_mon+1, current_datetime_struct->tm_mday);
}

std::string CDatetimeUtils::to_current_date(struct tm* current_datetime_struct, const char* format)
{
    char date_buffer[sizeof("YYYY-MM-DD")+100];
    to_current_date(current_datetime_struct, date_buffer, sizeof(date_buffer), format);
    return date_buffer;
}

void CDatetimeUtils::to_current_time(struct tm* current_datetime_struct, char* time_buffer, size_t time_buffer_size, const char* format)
{
    snprintf(time_buffer, time_buffer_size
        ,format
        ,current_datetime_struct->tm_hour, current_datetime_struct->tm_min, current_datetime_struct->tm_sec);
}

std::string CDatetimeUtils::to_current_time(struct tm* current_datetime_struct, const char* format)
{
    char time_buffer[sizeof("HH:SS:MM")+100];
    to_current_date(current_datetime_struct, time_buffer, sizeof(time_buffer), format);
    return time_buffer;
}

void CDatetimeUtils::to_current_year(struct tm* current_datetime_struct, char* year_buffer, size_t year_buffer_size)
{
    snprintf(year_buffer, year_buffer_size, "%04d", current_datetime_struct->tm_year+1900);
}

std::string CDatetimeUtils::to_current_year(struct tm* current_datetime_struct)
{
    char year_buffer[sizeof("YYYY")];
    to_current_year(current_datetime_struct, year_buffer, sizeof(year_buffer));
    return year_buffer;
}

void CDatetimeUtils::to_current_month(struct tm* current_datetime_struct, char* month_buffer, size_t month_buffer_size)
{
    snprintf(month_buffer, month_buffer_size, "%d", current_datetime_struct->tm_mon+1);
}

std::string CDatetimeUtils::to_current_month(struct tm* current_datetime_struct)
{
    char month_buffer[sizeof("MM")];
    to_current_month(current_datetime_struct, month_buffer, sizeof(month_buffer));
    return month_buffer;
}

void CDatetimeUtils::to_current_day(struct tm* current_datetime_struct, char* day_buffer, size_t day_buffer_size)
{
    snprintf(day_buffer, day_buffer_size, "%d", current_datetime_struct->tm_mday);
}

std::string CDatetimeUtils::to_current_day(struct tm* current_datetime_struct)
{
    char day_buffer[sizeof("DD")];
    to_current_day(current_datetime_struct, day_buffer, sizeof(day_buffer));
    return day_buffer;
}

void CDatetimeUtils::to_current_hour(struct tm* current_datetime_struct, char* hour_buffer, size_t hour_buffer_size)
{
    snprintf(hour_buffer, hour_buffer_size, "%d", current_datetime_struct->tm_hour);
}

std::string CDatetimeUtils::to_current_hour(struct tm* current_datetime_struct)
{
    char hour_buffer[sizeof("HH")];
    to_current_hour(current_datetime_struct, hour_buffer, sizeof(hour_buffer));
    return hour_buffer;
}

void CDatetimeUtils::to_current_minite(struct tm* current_datetime_struct, char* minite_buffer, size_t minite_buffer_size)
{
    snprintf(minite_buffer, minite_buffer_size, "%d", current_datetime_struct->tm_min);
}

std::string CDatetimeUtils::to_current_minite(struct tm* current_datetime_struct)
{
    char minite_buffer[sizeof("MM")];
    to_current_minite(current_datetime_struct, minite_buffer, sizeof(minite_buffer));
    return minite_buffer;
}

void CDatetimeUtils::to_current_second(struct tm* current_datetime_struct, char* second_buffer, size_t second_buffer_size)
{
    snprintf(second_buffer, second_buffer_size, "%d", current_datetime_struct->tm_sec);
}

std::string CDatetimeUtils::to_current_second(struct tm* current_datetime_struct)
{
    char second_buffer[sizeof("SS")];
    to_current_second(current_datetime_struct, second_buffer, sizeof(second_buffer));
    return second_buffer;
}

bool CDatetimeUtils::datetime_struct_from_string(const char* str, struct tm* datetime_struct)
{
    const char* tmp_str = str;

#ifdef _XOPEN_SOURCE
    return strptime(tmp_str, "%Y-%m-%d %H:%M:%S", datetime_struct) != NULL;
#else
    size_t str_len = strlen(tmp_str);
    if (str_len != sizeof("YYYY-MM-DD HH:MM:SS")-1) return false;
    if ((tmp_str[4] != '-')
     || (tmp_str[7] != '-')
     || (tmp_str[10] != ' ')
     || (tmp_str[13] != ':')
     || (tmp_str[16] != ':'))
        return false;

    using namespace util;
    if (!CStringUtils::string2int32(tmp_str, datetime_struct->tm_year, sizeof("YYYY")-1, false)) return false;
    if ((datetime_struct->tm_year > 3000) || (datetime_struct->tm_year < 1900)) return false;

    tmp_str += sizeof("YYYY");
    if (!CStringUtils::string2int32(tmp_str, datetime_struct->tm_mon, sizeof("MM")-1, true)) return false;
    if ((datetime_struct->tm_mon > 12) || (datetime_struct->tm_mon < 1)) return false;

    tmp_str += sizeof("MM");
    if (!CStringUtils::string2int32(tmp_str, datetime_struct->tm_mday, sizeof("DD")-1, true)) return false;
    if (datetime_struct->tm_mday < 1) return false;

    // 闰年二月可以有29天
    if ((CDatetimeUtils::is_leap_year(datetime_struct->tm_year)) && (2 == datetime_struct->tm_mon) && (datetime_struct->tm_mday > 29))
        return false;
    else if (datetime_struct->tm_mday > 28)
        return false;

    tmp_str += sizeof("DD");
    if (!CStringUtils::string2int32(tmp_str, datetime_struct->tm_hour, sizeof("HH")-1, true)) return false;
    if ((datetime_struct->tm_hour > 24) || (datetime_struct->tm_hour < 0)) return false;

    tmp_str += sizeof("HH");
    if (!CStringUtils::string2int32(tmp_str, datetime_struct->tm_min, sizeof("MM")-1, true)) return false;
    if ((datetime_struct->tm_min > 60) || (datetime_struct->tm_min < 0)) return false;

    tmp_str += sizeof("MM");
    if (!CStringUtils::string2int32(tmp_str, datetime_struct->tm_sec, sizeof("SS")-1, true)) return false;
    if ((datetime_struct->tm_sec > 60) || (datetime_struct->tm_sec < 0)) return false;

    datetime_struct->tm_isdst = 0;
    datetime_struct->tm_wday  = 0;
    datetime_struct->tm_yday  = 0;

    // 计算到了一年中的第几天
    for (int i=1; i<=datetime_struct->tm_mon; ++i)
    {
        if (i == datetime_struct->tm_mon)
        {
            // 刚好是这个月
            datetime_struct->tm_yday += datetime_struct->tm_mday;
        }
        else
        {
            // 1,3,5,7,8,10,12
            if ((1 == i) || (3 == i) || (5 == i) || (7 == i) || (8 == i) || (10 == i) || (12 == i))
            {
                datetime_struct->tm_yday += 31;
            }
            else if (2 == i)
            {
                if (CDatetimeUtils::is_leap_year(datetime_struct->tm_year))
                    datetime_struct->tm_yday += 29;
                else
                    datetime_struct->tm_yday += 28;
            }
            else
            {
                datetime_struct->tm_yday += 30;
            }
        }
    }

    // 月基数
    static int leap_month_base[] = { -1, 0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6 };
    static int common_month_base[] = { -1, 0, 3, 3, 6, 1, 4, 0, 3, 5, 0, 3, 5 };

    int year_base;
    int *month_base;
    if (CDatetimeUtils::is_leap_year(datetime_struct->tm_year))
    {
         year_base = 2;
         month_base = leap_month_base;
    }
    else
    {
         year_base = 1;
         month_base = common_month_base;
    }

    // 计算星期几
    datetime_struct->tm_wday = (datetime_struct->tm_year
                             +  datetime_struct->tm_year / 4
                             +  datetime_struct->tm_year / 400
                             -  datetime_struct->tm_year / 100
                             -  year_base
                             +  month_base[datetime_struct->tm_mon]
                             +  datetime_struct->tm_mday) / 7;

    // 年月处理
    datetime_struct->tm_mon -= 1;
    datetime_struct->tm_year -= 1900;
    return true;
#endif // _XOPEN_SOURCE
}

bool CDatetimeUtils::datetime_struct_from_string(const char* str, time_t* datetime)
{
    struct tm datetime_struct;
    if (!datetime_struct_from_string(str, &datetime_struct)) return false;

    *datetime = mktime(&datetime_struct);
    return true;
}

std::string CDatetimeUtils::to_string(time_t datetime, const char* format)
{
    struct tm result;
    localtime_r(&datetime, &result);

    char datetime_buffer[sizeof("YYYY-MM-DD HH:SS:MM")+100];
    (void)snprintf(datetime_buffer, sizeof(datetime_buffer)
        ,format
        ,result.tm_year+1900, result.tm_mon+1, result.tm_mday
        ,result.tm_hour, result.tm_min, result.tm_sec);

    return datetime_buffer;
}

std::string CDatetimeUtils::to_datetime(time_t datetime, const char* format)
{
    return CDatetimeUtils::to_string(datetime);
}

std::string CDatetimeUtils::to_date(time_t datetime, const char* format)
{
    struct tm result;
    localtime_r(&datetime, &result);

    char date_buffer[sizeof("YYYY-MM-DD")+100];
    (void)snprintf(date_buffer, sizeof(date_buffer)
        ,format
        ,result.tm_year+1900, result.tm_mon+1, result.tm_mday);

    return date_buffer;
}

std::string CDatetimeUtils::to_time(time_t datetime, const char* format)
{
    struct tm result;
    localtime_r(&datetime, &result);

    char time_buffer[sizeof("HH:SS:MM")+100];
    (void)snprintf(time_buffer, sizeof(time_buffer)
        ,format
        ,result.tm_hour, result.tm_min, result.tm_sec);

    return time_buffer;
}

int64_t CDatetimeUtils::get_current_microseconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<int64_t>(tv.tv_sec) * 1000000 + static_cast<int64_t>(tv.tv_usec);
}

////////////////////////////////////////////////////////////////////////////////
std::string today(const char* format)
{
    return CDatetimeUtils::get_current_date(format);
}

std::string yesterday(const char* format)
{
    time_t now = time(NULL);
    return CDatetimeUtils::to_date(now-(3600*24), format);
}

std::string tomorrow(const char* format)
{
    time_t now = time(NULL);
    return CDatetimeUtils::to_date(now+(3600*24), format);
}

void get_formatted_current_datetime(char* datetime_buffer, size_t datetime_buffer_size, bool with_milliseconds)
{
    struct timeval current;
    gettimeofday(&current, NULL);
    time_t current_seconds = current.tv_sec;

    struct tm result;
    localtime_r(&current_seconds, &result);

    if (with_milliseconds)
    {
        snprintf(datetime_buffer, datetime_buffer_size
            ,"%04d-%02d-%02d %02d:%02d:%02d/%u"
            ,result.tm_year+1900, result.tm_mon+1, result.tm_mday
            ,result.tm_hour, result.tm_min, result.tm_sec, (unsigned int)(current.tv_usec/1000));
    }
    else
    {
        snprintf(datetime_buffer, datetime_buffer_size
            ,"%04d-%02d-%02d %02d:%02d:%02d"
            ,result.tm_year+1900, result.tm_mon+1, result.tm_mday
            ,result.tm_hour, result.tm_min, result.tm_sec);
    }
}

std::string get_formatted_current_datetime(bool with_milliseconds)
{
    char datetime_buffer[sizeof("YYYY-MM-DD hh:mm:ss/0123456789")];
    size_t datetime_buffer_size = sizeof(datetime_buffer);
    get_formatted_current_datetime(datetime_buffer, datetime_buffer_size, with_milliseconds);

    return datetime_buffer;
}

SYS_NAMESPACE_END
