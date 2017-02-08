//=================================================================================================
//                    Copyright (C) 2017 Olivier Mallet - All Rights Reserved                      
//=================================================================================================

#include "QuotesDB.hpp"

int main()
{
   // creating a connection to Oanda server
   qdb::OandaAPI conn("practice");

   // downloading data from the 1st of Janurary 2007 and recording them in QuotesDB database
   conn.initAllTabs("QuotesDB","2007-01-01 00:00:00");
   // it will then download data for every pair (INSTRUMENT,GRANULARITY) defined in QuotesDB.hpp
   
   // you can the update later the database by simply doing:
   //conn.updateAllTabs("QuotesDB");

   // connecting to QuotesDB database for reading data
   qdb::DataBase db("QuotesDB");

   // reading the last 10 recorded Bars from table EUR_USD_D 
   std::vector<qdb::Bar> data = db.read_table("EUR_USD_D",10);

   // outputting the data
   for (int i = 0; i < data.size(); ++i) {
      data[i].print();
   }
}
