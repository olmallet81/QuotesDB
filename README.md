# QuotesDB
A C++ interface for building your own database of historical market quotes.

# Description
QuotesDB allows the user to download historical data from [Oanda](http://developer.oanda.com/) using their REST API and record them into a MySQL database. I am not recommending you to use this broker, I choose them only for the purpose of this project as their API is well documented and it is very quick and straightforward to create a demo account allowing you to use this API.

# How to use it

To use QuotesDB you will need:
- [POCO C++ library](https://pocoproject.org/)
- [MySQL Server](https://dev.mysql.com/downloads/mysql/)
- [MySQL Connector/C++](https://dev.mysql.com/downloads/connector/cpp/1.1.html)
- A demo account on [Oanda](http://pages.oanda.com/forex-cfd-trading.html?gclid=COzg84aLgNICFRKeGwodmDgHeg&gclsrc=aw.ds)

Once registered to Oanda you will get an acccount id and an access token allowing you to download historical data from their server.

You need to create a database in MySQL for recording the downloaded historical data from Oanda. I called mine QuotesDB.

You will then configurate the header file QuotedDB.hpp with your own parameters for the connection to MySQL database, to Oanda server and the desired instruments and granularities.

# Example

```C++
#include "QuotesDB.hpp"

int main()
{
   // creating a connection to Oanda server
   qdb::OandaAPI conn("practice");

   // downloading data from the 1st of Janurary 2007 and recording them in QuotesDB database
   conn.initAllTabs("QuotesDB","2007-01-01 00:00:00");
   // it will then download data for every pair (INSTRUMENT,GRANULARITY) defined in QuotesDB.hpp
   
   // you can update later all the tables in the database by simply doing:
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
```


# Compilation

You will need to link to Poco and MySQL librairies to compile QuotesDB.
```
g++ -std=c++11 -O3 -Wall example.cpp -o run -lPocoNet -lPocoNetSSL -lPocoFoundation -lPocoJSON -lmysqlcppconn
```
Poco and MySQL need to be on your compiler path otherwise it will not find the required headers and libraries.

# Ouput

```
2017-02-06 22:00:00  1.07444  1.07544  1.07486  1.07556  1.06555  1.06568  1.06822  1.06837   24736
2017-02-05 22:00:00  1.07866  1.07929  1.07881  1.07954  1.07047  1.07065  1.07491  1.07516   26266
2017-02-02 22:00:00  1.07564  1.07628   1.0797  1.07983  1.07088  1.07152    1.078    1.079   36782
2017-02-01 22:00:00   1.0768  1.07705  1.08282  1.08297  1.07554  1.07569  1.07574  1.07593   32335
2017-01-31 22:00:00  1.07959  1.08001  1.08068  1.08081  1.07263  1.07356  1.07675  1.07703   35995
2017-01-30 22:00:00  1.06931  1.06998  1.08116  1.08132  1.06839  1.06853  1.07966  1.07984   42940
2017-01-29 22:00:00  1.07182  1.07282  1.07396  1.07412  1.06198  1.06211  1.06942  1.06971   28475
2017-01-26 22:00:00  1.06774   1.0682  1.07243  1.07259  1.06576  1.06591  1.06931  1.07031   29190
2017-01-25 22:00:00  1.07464   1.0753  1.07649  1.07663  1.06572  1.06586  1.06807  1.06841   30854
2017-01-24 22:00:00  1.07274  1.07338  1.07691  1.07705  1.07105   1.0712  1.07465  1.07501   28624
```
The columns correspond to the Bar opening date, the open bid and ask, the high bid and ask, the low bid and ask, the close bid and ask and the tick volume.


