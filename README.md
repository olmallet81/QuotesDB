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
