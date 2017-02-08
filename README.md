# QuotesDB
A C++ interface for building your own database of historical market quotes.

# Description
QuotesDB allows the user to download historical data from [Oanda](http://developer.oanda.com/) using their REST API and record them into a MySQL database. I am not doing any advertising for this broker, I choose them only for the purpose of this project as their API is well documented and it is very quick and straightforward to create a demo account allowing the use of this API.

# How to use it

To use QuotesDB you will need:
- [POCO C++ library](https://pocoproject.org/)
- [MySQL Server](https://dev.mysql.com/downloads/mysql/)
- [MySQL Connector/C++](https://dev.mysql.com/downloads/connector/cpp/1.1.html)
- A demo account on [Oanda](http://pages.oanda.com/forex-cfd-trading.html?gclid=COzg84aLgNICFRKeGwodmDgHeg&gclsrc=aw.ds)

Once registered to Oanda you will get an acccount id and an access token allowing you to download historical data from their server.

You need to create a database in MySQL for recording the downloaded historical data from Oanda. I called mine QuotesDB.

You will then configurate the header file QuotedDB.hpp with your own parameters for the connection to MySQL database, to Oanda server and the desired instruments and granularities.

# Compilation

You will need to link to Poco and MySQL librairies to compile QuotesDB.
```
g++ -std=c++11 -O3 -Wall example.cpp -o run -lPocoNet -lPocoNetSSL -lPocoFoundation -lPocoJSON -lmysqlcppconn
```
Poco and MySQL need to be on your compiler path otherwise it will not find the required headers and libraries.

# Example

```C++
#include "QuotesDB.hpp"

int main()
{
   // creating a connection to Oanda server
   qdb::OandaAPI conn("practice");

   // downloading data and recording them to QuotesDB database
   conn.initTab("QuotesDB","EUR_USD","D","2007-01-01 00:00:00");
   // it will download all daily quotes of EURUSD from the 1st of January 2007

   // or you can do directly
   //conn.initAllTabs("QuotesDB","2007-01-01 00:00:00");
   // it will then download data for all pairs (INSTRUMENT,GRANULARITY) defined in QuotesDB.hpp

   // connecting to QuotesDB database for reading data
   qdb::DataBase db("QuotesDB");

   // reading the last 10 recorded Bars from table EUR_USD_D 
   std::vector<qdb::Bar> data = db.read_table("EUR_USD_D",10);

   // outputting the data
   for (int i = 0; i < data.size(); ++i) {
      data[i].print();
   }
}
```

