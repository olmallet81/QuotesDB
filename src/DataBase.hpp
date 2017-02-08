//=================================================================================================
//                    Copyright (C) 2017 Olivier Mallet - All Rights Reserved                      
//=================================================================================================

#ifndef DATABASE_HPP
#define DATABASE_HPP

namespace qdb {

//=================================================================================================

// class for interacting with MySQL database

class DataBase
{
public:
   // parameter constructor
   DataBase(const std::string& db_name);
   // create or re-initialize a table in database to contain Bars
   void create_table(const std::string& tab_name);
   // write to table in database appending new Bars
   void write_table(const std::string& tab_name, const std::vector<Bar>& data, int start = 0);
   // read table from database and record the data into a vector of Bars
   std::vector<Bar> read_table(const std::string& tab_name);
   // read table from database from a given start date (included)
   std::vector<Bar> read_table(const std::string& tab_name, const std::string& start_date);
   // read table from database between a given start date and a given end date (included)
   std::vector<Bar> read_table(const std::string& tab_name, const std::string& start_date, const std::string& end_date);
   // get last n rows from table in database (most recent will be first element in vector)
   std::vector<Bar> read_table(const std::string& tab_name, unsigned n);
   // get last row from table in database 
   Bar get_last_row(const std::string& tab_name);


private:
   std::unique_ptr<sql::Connection> con;
   std::unique_ptr<sql::Statement> stmt;
   std::unique_ptr<sql::PreparedStatement> pstmt;
   std::unique_ptr<sql::ResultSet> res;

   // fill vector of Bars with data from table in database
   void getData(std::vector<Bar>& data);
   // output caught exception details
   void exception_caught(sql::SQLException &e);
};

/*-------------------------------------------------------------------------------------------------*/

// parameter constructor
DataBase::DataBase(const std::string& db_name) 
{
   try {
      // creating a connection 
      sql::Driver* driver = get_driver_instance();
      con.reset(driver->connect(URL,USER,PASSWORD)); 
      // connecting to database
      con->setSchema(db_name);  
      // initializing statement
      stmt.reset(con->createStatement());      
   } catch (sql::SQLException &e) {
      exception_caught(e);
   }  
}

/*-------------------------------------------------------------------------------------------------*/

// create or re-initialize a table in database to contain Bars
void DataBase::create_table(const std::string& tab_name) 
{
   try {
      stmt->execute("DROP TABLE IF EXISTS " + tab_name);
      stmt->execute("CREATE TABLE " + tab_name + " (date INTEGER UNSIGNED,  "
                                                 "  openBid FLOAT(8,5),     "
                                                 "  openAsk FLOAT(8,5),     " 
                                                 "  highBid FLOAT(8,5),     " 
                                                 "  highAsk FLOAT(8,5),     " 
                                                 "  lowBid FLOAT(8,5),      " 
                                                 "  lowAsk FLOAT(8,5),      " 
                                                 "  closeBid FLOAT(8,5),    " 
                                                 "  closeAsk FLOAT(8,5),    "  
                                                 "  volume INTEGER UNSIGNED,"
                                                 "  PRIMARY KEY(date))");
   } catch (sql::SQLException &e) {
      exception_caught(e);
   }   
}

/*-------------------------------------------------------------------------------------------------*/

// write to table in database appending vector of Bars starting from position start in vector
void DataBase::write_table(const std::string& tab_name, const std::vector<Bar>& data, int start) 
{
   try {
      pstmt.reset(con->prepareStatement("INSERT INTO " + tab_name + "(date,     "
                                                                    " openBid,  "
                                                                    " openAsk,  "
                                                                    " highBid,  "
                                                                    " highAsk,  " 
                                                                    " lowBid,   "
                                                                    " lowAsk,   "
                                                                    " closeBid, "
                                                                    " closeAsk, "
                                                                    " volume    "
                                                                    ") VALUES (?,?,?,?,?,?,?,?,?,?)"));
      // writing to table
      for (int i = start; i < data.size(); ++i) {
         pstmt->setInt(1, data[i].date);
         pstmt->setDouble(2, data[i].openBid); 
         pstmt->setDouble(3, data[i].openAsk); 
         pstmt->setDouble(4, data[i].highBid); 
         pstmt->setDouble(5, data[i].highAsk); 
         pstmt->setDouble(6, data[i].lowBid); 
         pstmt->setDouble(7, data[i].lowAsk); 
         pstmt->setDouble(8, data[i].closeBid); 
         pstmt->setDouble(9, data[i].closeAsk); 
         pstmt->setInt(10, data[i].volume);         
         pstmt->execute();
      }

   } catch (sql::SQLException &e) {
      exception_caught(e);
   }   
}

/*-------------------------------------------------------------------------------------------------*/

// read full table from database and record the data into a vector of Bars
std::vector<Bar> DataBase::read_table(const std::string& tab_name) 
{
   std::vector<Bar> data;

   try {
      res.reset(stmt->executeQuery("SELECT * FROM " + tab_name));

      getData(data);

   } catch (sql::SQLException &e) {
      exception_caught(e);
   }

   return data; 
}

/*-------------------------------------------------------------------------------------------------*/

// read table from database from a given start date (included)
std::vector<Bar> DataBase::read_table(const std::string& tab_name, const std::string& start_date)
{
   std::vector<Bar> data;

   // converting date to seconds since epoch
   std::string start = std::to_string(string_to_sec(start_date));

   try {
      res.reset(stmt->executeQuery("SELECT * FROM " + tab_name + " WHERE date >= " + start));

      getData(data);

   } catch (sql::SQLException &e) {
      exception_caught(e);
   }

   return data; 
}

/*-------------------------------------------------------------------------------------------------*/

// read table from database between a given start date and a given end date (included)
std::vector<Bar> DataBase::read_table(const std::string& tab_name, const std::string& start_date, const std::string& end_date)
{
   std::vector<Bar> data;

   // converting dates to seconds since epoch
   std::string start = std::to_string(string_to_sec(start_date));
   std::string end = std::to_string(string_to_sec(end_date));

   try {
      res.reset(stmt->executeQuery("SELECT * FROM " + tab_name + " WHERE date >= " + start + " AND date <= " + end));

      getData(data);

   } catch (sql::SQLException &e) {
      exception_caught(e);
   }

   return data; 
}

/*-------------------------------------------------------------------------------------------------*/

// get last n rows from table in database (most recent Bar will be first in vector)
std::vector<Bar> DataBase::read_table(const std::string& tab_name, unsigned n) 
{
   std::vector<Bar> data;

   try {
      res.reset(stmt->executeQuery("SELECT * FROM " + tab_name + " ORDER BY date DESC LIMIT " + std::to_string(n)));

      getData(data);

   } catch (sql::SQLException &e) {
      exception_caught(e);
   }

   return data;
}

/*-------------------------------------------------------------------------------------------------*/

// get last row from table in database 
Bar DataBase::get_last_row(const std::string& tab_name) 
{
   try {
      res.reset(stmt->executeQuery("SELECT * FROM " + tab_name + " ORDER BY date DESC LIMIT 1"));
      res->next();

      return Bar(res->getInt("date"),
                 res->getDouble("openBid"),
                 res->getDouble("openAsk"),
                 res->getDouble("highBid"),
                 res->getDouble("highAsk"),
                 res->getDouble("lowBid"),
                 res->getDouble("lowAsk"),
                 res->getDouble("closeBid"),
                 res->getDouble("closeAsk"),
                 res->getInt("volume"));

   } catch (sql::SQLException &e) {
      exception_caught(e);
   }
}

/*-------------------------------------------------------------------------------------------------*/

// fill vector of Bars with data from table in database
void DataBase::getData(std::vector<Bar>& data)
{
   while (res->next()) {
      data.emplace_back(res->getInt("date"),
                        res->getDouble("openBid"),
                        res->getDouble("openAsk"),
                        res->getDouble("highBid"),
                        res->getDouble("highAsk"),
                        res->getDouble("lowBid"),
                        res->getDouble("lowAsk"),
                        res->getDouble("closeBid"),
                        res->getDouble("closeAsk"),
                        res->getInt("volume"));
   }
}

/*-------------------------------------------------------------------------------------------------*/

// output caught exception details
void DataBase::exception_caught(sql::SQLException &e) 
{
   std::cout << "ERROR: SQLException in " << __FILE__;
   std::cout << " (" << __FUNCTION__ << ") on line " << __LINE__ << "\n";
   std::cout << "ERROR: " << e.what();
   std::cout << " (MySQL error code: " << e.getErrorCode();
   std::cout << ", SQLState: " << e.getSQLState() << ")" << "\n";
}

//=================================================================================================

}

#endif

