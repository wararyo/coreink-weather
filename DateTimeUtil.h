#ifndef _DATETIMEUTIL_H_
#define _DATETIMEUTIL_H_

#include "time.h"
#include "M5CoreInk.h"

String dateToString(RTC_Date date) {
    char monthStr[5], dayStr[5];
    sprintf(monthStr, "%02d", date.Month);
    sprintf(dayStr, "%02d", date.Date);    
    return String(date.Year)+String("-")+String(monthStr)+String("-")+String(dayStr);
}

String dateTimeToString(RTC_DateTypeDef RtcDate, RTC_TimeTypeDef RtcTime) {
    return String(RtcDate.Month)+String("/")+String(RtcDate.Date) + String(" ") + String(RtcTime.Hours)+String(":")+String(RtcTime.Minutes);
}

void convertDateFromRTC(RTC_DateTypeDef RtcDate, tm *date) {
    date->tm_year = RtcDate.Year - 1900;
    date->tm_mon = RtcDate.Month - 1;
    date->tm_mday = RtcDate.Date;
    date->tm_wday = RtcDate.WeekDay;
    date->tm_hour = 0;
    date->tm_min = 0;
    date->tm_sec = 0;
    date->tm_isdst = -1;
}

void convertDateToRTC(RTC_DateTypeDef *RtcDate, tm date) {
    RtcDate->Year = date.tm_year + 1900;
    RtcDate->Month = date.tm_mon + 1;
    RtcDate->Date = date.tm_mday;
    RtcDate->WeekDay = date.tm_wday;
}

void convertTimeToRTC(RTC_TimeTypeDef *RtcTime, tm time) {
    RtcTime->Minutes = time.tm_min;
    RtcTime->Seconds = time.tm_sec;
    RtcTime->Hours = time.tm_hour;
}

// 日付の加算を行う
// offset = 1 の時、翌日を取得
void offsetDate(RTC_DateTypeDef *RtcDate, int offset = 1) {
    // RTC_DateTypeDef -> tm -> time_t の順に変換、日付を加算し、逆の順序で戻す
    tm date;
    convertDateFromRTC(*RtcDate, &date);
    time_t timestamp = mktime(&date);
    timestamp += 86400 * offset; // 86400 = 60*60*24
    date = *(localtime(&timestamp));
    convertDateToRTC(RtcDate, date);
}

#endif _DATETIMEUTIL_H_
