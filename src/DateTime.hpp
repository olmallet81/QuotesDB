#ifndef DATETIME_HPP
#define DATETIME_HPP

namespace qdb {

//================================================================================================= 

// convert string date in format YYYY-mm-dd HH:MM:SS to Oanda format
inline std::string string_to_oanda(const std::string& date)
{
   return date.substr(0,10)+"T"+date.substr(11,2)+"%3A"+date.substr(14,2)+"%3A"+date.substr(17,2)+"Z";
}

/*-------------------------------------------------------------------------------------------------*/

// convert Oanda date to string date in format YYYY-mm-dd HH:MM:SS
inline std::string oanda_to_string(const std::string& date)
{
   return date.substr(0,10)+" "+date.substr(11,2)+":"+date.substr(16,2)+":"+date.substr(21,2);
}

/*-------------------------------------------------------------------------------------------------*/

//convert string date to time structure
tm string_to_tm(const std::string& date) 
{
   tm d = {};
   d.tm_year = std::stoi(date.substr(0,4).c_str())-1900;
   d.tm_mon = std::stoi(date.substr(5,2).c_str())-1;
   d.tm_mday = std::stoi(date.substr(8,2).c_str());
   d.tm_hour = std::stoi(date.substr(11,2).c_str());
   d.tm_min = std::stoi(date.substr(14,2).c_str());
   d.tm_sec = std::stoi(date.substr(17,2).c_str());
   d.tm_isdst = -1;

   return d;
}

/*-------------------------------------------------------------------------------------------------*/

// convert time structure to string date in format YYYY-mm-dd HH:MM:SS
std::string tm_to_string(tm& d) 
{
   char buf[20];
   time_t t = timegm(&d);
   tm* p = gmtime(&t);
   strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",p);
   return buf;
}

/*-------------------------------------------------------------------------------------------------*/

// convert seconds since epoch to string date in format YYYY-mm-dd HH:MM:SS
std::string sec_to_string(unsigned secs) 
{
   char buf[20];
   time_t t = static_cast<time_t>(secs);
   tm* p = gmtime(&t);
   strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",p);
   return buf;
}

/*-------------------------------------------------------------------------------------------------*/

// convert string date in format YYYY-mm-dd HH:MM:SS to UTC seconds since epoch
inline time_t string_to_sec(const std::string& date)
{
    tm d = string_to_tm(date);
    return timegm(&d);
}

/*-------------------------------------------------------------------------------------------------*/

// get the current UTC time
std::string get_utc_time()
{
   char buf[20];
   time_t t;
   time(&t); // current local time
   tm *p = gmtime(&t); // convert local time to UTC time
   strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",p);
   return buf;
}

/*-------------------------------------------------------------------------------------------------*/

// get the current US/Eastern time
std::string get_est_time()
{
   char buf[20];
   setenv("TZ","EST5EDT4",1);
   time_t t;
   time(&t); //current local time
   tm *p = localtime(&t);
   unsetenv("TZ");
   strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",p);
   return buf;
}

/*-------------------------------------------------------------------------------------------------*/

// get US/Eastern standard time from UTC time
std::string utc_to_est(const std::string& date)
{
   char buf[20];
   tm d = string_to_tm(date);
   time_t t = timegm(&d);  // seconds to epoch for UTC 
   setenv("TZ","EST5EDT4",1);
   tm *p = localtime(&t);  // US/Eastern local time
   unsetenv("TZ");
   strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",p);
   return buf;
}

/*-------------------------------------------------------------------------------------------------*/

// get UTC time from US/Eastern local time
std::string est_to_utc(const std::string& date)
{  
   char buf[20];
   tm d = string_to_tm(date);
   setenv("TZ","EST5EDT4",1);
   time_t t = mktime(&d);  // seconds to epoch for US/Eastern
   unsetenv("TZ");
   tm* p = gmtime(&t);  // UTC time
   strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",p);
   return buf;
}

/*-------------------------------------------------------------------------------------------------*/

// checking wether a US/Eastern date is on a week-end, x-mas or new-year
bool is_day_off(const std::string& dt)
{
   tm d = string_to_tm(dt);
   time_t t = timegm(&d);      
   tm* p = gmtime(&t);   
       
   // remove week-ends
   if ((p->tm_wday==5 && p->tm_hour>=17) || p->tm_wday==6 || (p->tm_wday==0 && p->tm_hour<17)) {
      return true;
   }
   // remove x-mas
   else if ((p->tm_mon==11 && p->tm_mday==24 && p->tm_hour>=17) || (p->tm_mon==11 && p->tm_mday==25 && p->tm_hour<17)) {
      return true;
   }
   // remove new-year
   else if ((p->tm_mon==11 && p->tm_mday==31 && p->tm_hour>=17) || (p->tm_mon==00 && p->tm_mday==01 && p->tm_hour<17)) {
      return true;
   } 
   else {
      return false;
   }
}

/*-------------------------------------------------------------------------------------------------*/

// get a range of dates in Oanda format for downloading historical data by blocks
// as Oanda is limited to 5000 data per request
std::vector<std::string> getDates(const std::string& start, const std::string& end, const std::string& granularity)
{
   std::vector<std::string> dates;

   int nb_secs;

   // converting granularity into seconds
   if (granularity.substr(0,1) == "M") {
      if (granularity.size() == 2) {
         nb_secs = std::stoi(granularity.substr(1,1)) * 60; 
      }
      else if (granularity.size() == 3) {
         nb_secs = std::stoi(granularity.substr(1,2)) * 60; 
      }
   }
   else if (granularity.substr(0,1) == "H") {
      if (granularity.size() == 2) {
         nb_secs = std::stoi(granularity.substr(1,1)) * 3600; 
      }
      else if (granularity.size() == 3) {
         nb_secs = std::stoi(granularity.substr(1,2)) * 3600; 
      }
   } 
   else if (granularity == "D") {
      nb_secs = 86400;
   }

   // converting start and end date into seconds since epoch
   tm d = string_to_tm(start);
   time_t start_t = timegm(&d);
   d = string_to_tm(end);
   time_t end_t = timegm(&d);

   // computing expected number of bars on time interval [start,end]
   int nb = (end_t - start_t) / nb_secs;
   // intializing dates
   dates.push_back(string_to_oanda(start));

   int i = 5000;

   while (i <= nb) {
      start_t += 5000 * nb_secs;
      tm* p = gmtime(&start_t);
      dates.push_back(string_to_oanda(tm_to_string(*p)));
      i += 5000;
   }
   // adding end date if not reached
   if (start_t < end_t) {
      dates.push_back(string_to_oanda(tm_to_string(d)));
   }

   return dates;
}

//================================================================================================= 

}

#endif

