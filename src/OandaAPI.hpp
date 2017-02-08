//=================================================================================================
//                    Copyright (C) 2017 Olivier Mallet - All Rights Reserved                      
//=================================================================================================

#ifndef OANDAAPI_H
#define OANDAAPI_H

namespace qdb {

//================================================================================================= 

// class for interacting with Oanda server

class OandaAPI
{
public:
   // parameter constructor, connect to Oanda server
   OandaAPI(const std::string& environment); 
   // send request to Oanda server
   std::string request(const std::string& endpoint) const;
   // get historical data from Oanda
   void getHistoData(const std::string& instrument, const std::vector<std::string>& parameters, std::vector<Bar>& data) const;
   // initialize table in database for one pair instrument & granularity
   void initTab(const std::string& db_name, const std::string& instrument, const std::string& granularity, const std::string& start_date) const;
   // update table in database for one pair instrument & granularity
   void updateTab(const std::string& db_name, const std::string& instrument, const std::string& granularity) const;
   // initialize all tables in database
   void initAllTabs(const std::string& db_name, const std::string& start_date) const;
   // update all tables in database
   void updateAllTabs(const std::string& db_name) const;
   // return all instruments and details available in Oanda
   std::string getInstruments() const;

private:
   Poco::Net::HTTPSClientSession *session;    
   std::string environment;
   std::string domain;
   std::string access_token;
   bool keep_alive;

   // Oanda available granularities
   const std::array<std::string, 22> granularities = {"S5","S10","S15","S30",
                                                      "M1","M2","M3","M4","M5","M10","M15","M30",
                                                      "H1","H2","H3","H4","H6","H8","H12",
                                                      "D","W","M"};
};

/*-------------------------------------------------------------------------------------------------*/

// parameter constructor, connect to Oanda server
OandaAPI::OandaAPI(const std::string& environment) 
{
   this->environment = environment;
   if (environment == std::string("practice")) {
      this->domain = "https://api-fxpractice.oanda.com";
   }
   else if (environment == std::string("live")) {
      this->domain = "https://api-fxtrade.oanda.com"; 
   }

   try {
      // context information for a Secure Socket Layer (SSL) client
      const Poco::Net::Context::Ptr context = new Poco::Net::Context(
                                                  Poco::Net::Context::CLIENT_USE,"","","",
                                                  Poco::Net::Context::VERIFY_NONE,9,false,
                                                  "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
      // implementing the client-side of an HTTP Secure session
      Poco::URI uri(domain);
    	session = new Poco::Net::HTTPSClientSession(uri.getHost(),uri.getPort(),context);
      // keeping the connection alive
      session->setKeepAlive(true);
   }
   catch (const Poco::Exception &e) {
      std::cout << e.displayText() << "\n";
   }
}

/*-------------------------------------------------------------------------------------------------*/
    
// send request to Oanda server
std::string OandaAPI::request(const std::string& endpoint) const 
{
   // building URI 
   Poco::URI uri(domain + endpoint);
   // preparing path
   std::string path(uri.getPathAndQuery());
   if (path.empty()) path = "/";
   // sending request to receive data from server
   Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
   req.set("Authorization", std::string("Bearer ") + ACCESS_TOKEN);

   // sending request to Oanda server
   session->sendRequest(req);

   Poco::Net::HTTPResponse res;
   // getting response
   std::istream& rs = session->receiveResponse(res);
   // reading response
   std::istreambuf_iterator<char> eos;
   std::string content(std::istreambuf_iterator<char>(rs),eos);
   return content;
}

/*-------------------------------------------------------------------------------------------------*/

// get historical data from Oanda
void OandaAPI::getHistoData(const std::string& instrument, const std::vector<std::string>& parameters, std::vector<Bar>& data) const 
{
   std::string params;
   for (const auto& elem : parameters) params += "&" + elem;

   std::string endpoint("/v1/candles?instrument=" + instrument + params);
   std::string content;

   // getting start date in seconds since epoch
   unsigned start_t = string_to_sec(parameters[0].substr(6,19));

   try {
      // getting response content
      content = request(endpoint);
   } catch (const Poco::Exception& e) {
      std::cout << e.displayText() << "\n";
   }

   if (!content.empty()) {

      // parsing content returned
      Poco::JSON::Parser parser;
      Poco::Dynamic::Var result = parser.parse(content);
      Poco::JSON::Object::Ptr obj = result.extract<Poco::JSON::Object::Ptr>();
      Poco::JSON::Array::Ptr arr = obj->getArray("candles");

      // avoiding duplicate Bars using previous date
      std::string prev_date;

      // reading response
      for (int i = 0; i < arr->size(); ++i) {
         obj = arr->getObject(i);
         // converting datetime format
         std::string date = obj->getValue<std::string>("time");
         // adjusting date format
         date = date.substr(0,10) + " " + date.substr(11,8);
         unsigned date_t = string_to_sec(date);
         // the Bar has to verify the following conditions to be recorded:
         // Bar is not a day off 
         // Bar is complete 
         // Bar is not a duplicate of the previous one 
         // Bar date is equal or after the chosen starting date for downloading data
         if (!is_day_off(utc_to_est(date)) && (obj->getValue<bool>("complete") && prev_date != date && date_t >= start_t)) {
            // adding element
            data.emplace_back(date_t,
                              obj->getValue<float>("openBid"),
                              obj->getValue<float>("openAsk"),
                              obj->getValue<float>("highBid"),
                     			obj->getValue<float>("highAsk"),
                              obj->getValue<float>("lowBid"),
                              obj->getValue<float>("lowAsk"),
                              obj->getValue<float>("closeBid"),
                              obj->getValue<float>("closeAsk"),
                              obj->getValue<int>("volume"));     
         }
         prev_date = date;
      }
   }
}

/*-------------------------------------------------------------------------------------------------*/

// initialize table in database for one pair instrument & granularity
void OandaAPI::initTab(const std::string& db_name, const std::string& instrument, const std::string& granularity, const std::string& start_date) const
{
   // getting table name to write to
   std::string tab_name = instrument + "_" + granularity;
   // connecting to database table
   DataBase db(db_name);
   // creating table
   db.create_table(tab_name);
   // getting block dates for data download, start date included, end date excluded
   std::vector<std::string> dates = getDates(start_date, get_utc_time(), granularity);
   // container of Bars
   std::vector<Bar> data;

   // downloading data
   for (int i = 0; i < dates.size() - 1; ++i) {
      std::cout << "downloading " + instrument + " " + granularity + " data from " << oanda_to_string(dates[i]);
      std::cout << " to " << oanda_to_string(dates[i + 1]) << "...\n";
      
      getHistoData(instrument, {"start="+dates[i],"end="+dates[i+1],"candleFormat=bidask","granularity="+ granularity}, data);

      std::cout << "writing to table " + tab_name + "...\n";
      db.write_table(tab_name, data);
      // clearing vector
      data.clear();
   }
   std::cout << "all done for " + instrument + " " + granularity << "\n";
}

/*-------------------------------------------------------------------------------------------------*/

// update database for one pair instrument & granularity
void OandaAPI::updateTab(const std::string& db_name, const std::string& instrument, const std::string& granularity) const
{
   // getting table name to write to
   std::string tab_name = instrument + "_" + granularity;
   // connecting to database
   DataBase db(db_name);
   // getting last recorded Bar in table
   Bar x = db.get_last_row(tab_name);
   // getting block dates for data download
   std::vector<std::string> dates = getDates(sec_to_string(x.date), get_utc_time(), granularity);
   // container of Bars
   std::vector<Bar> data;

   // NB: As we start downloading from the last recorded Bar date 
   // we will get a duplicate Bar, we will skip it when writing to the table

   // downloading data
   for (int i = 0; i < dates.size() - 1; ++i) {
      std::cout << "downloading " + instrument + " " + granularity + " data from " << oanda_to_string(dates[i]);
      std::cout << " to " << oanda_to_string(dates[i + 1]) << "...\n";
      
      getHistoData(instrument, {"start="+dates[i],"end="+dates[i+1],"candleFormat=bidask","granularity="+ granularity}, data);

      std::cout << "writing to table " + tab_name + "...\n";

      if (data.size() > 1) {
         // skipping first Bar for avoiding duplicate
         db.write_table(tab_name, data, 1);
      }
      // clearing vector
      data.clear();
   }
   std::cout << "all done for " + instrument + " " + granularity << "\n";
}

/*-------------------------------------------------------------------------------------------------*/

// initialize all tables in database
void OandaAPI::initAllTabs(const std::string& db_name, const std::string& start_date) const
{
   for (const auto& instrument : INSTRUMENTS) {
      for (const auto& granularity : GRANULARITIES) {
         std::cout << "\n" << "-----------------------------------------------------------------------------" << "\n\n";
         initTab(db_name, instrument, granularity, start_date);
      }      
   }
   std::cout << "\n";
}

/*-------------------------------------------------------------------------------------------------*/

// update all tables in database
void OandaAPI::updateAllTabs(const std::string& db_name) const
{
   for (const auto& instrument : INSTRUMENTS) {
      for (const auto& granularity : GRANULARITIES) {
         std::cout << "\n" << "-----------------------------------------------------------------------------" << "\n\n";
         updateTab(db_name, instrument, granularity);
      }      
   }
   std::cout << "\n";
}

/*-------------------------------------------------------------------------------------------------*/

// return all instruments and details available in Oanda
std::string OandaAPI::getInstruments() const
{
   std::string endpoint("/v1/instruments?accountId=" + ACCOUNT_ID);

   try {
      return request(endpoint);
   } catch (const Poco::Exception& e) {
      std::cout << e.displayText() << "\n";
   }
}

//================================================================================================= 

}

#endif
