//=================================================================================================
//                    Copyright (C) 2017 Olivier Mallet - All Rights Reserved                      
//=================================================================================================

#ifndef BAR_HPP
#define BAR_HPP

namespace qdb {

//================================================================================================= 

struct Bar
{
   unsigned date;
   float openBid, openAsk;
   float highBid, highAsk;
   float lowBid, lowAsk;
   float closeBid, closeAsk;
   unsigned volume;

   // nullary constructor
   Bar() {}
   // parameter constructor
   Bar(unsigned date,float openBid,float openAsk,float highBid,float highAsk,float lowBid,float lowAsk,float closeBid,float closeAsk,unsigned volume);
   // get open price mid market
   float getOpen() const;
   // get high price mid market
   float getHigh() const; 
   // get low price mid market
   float getLow() const;
   // get close price mid market
   float getClose() const;
   // output bar
   void print() const;
};

/*-------------------------------------------------------------------------------------------------*/

// parameter constructor
Bar::Bar(unsigned date,float openBid,float openAsk,float highBid,float highAsk,float lowBid,float lowAsk,float closeBid,float closeAsk,unsigned volume) 
{
   this->date = date;
   this->openBid = openBid;
   this->openAsk = openAsk;
   this->highBid = highBid; 
   this->highAsk = highAsk; 
   this->lowBid = lowBid;
   this->lowAsk = lowAsk;
   this->closeBid = closeBid;
   this->closeAsk = closeAsk;
   this->volume = volume;
}

/*-------------------------------------------------------------------------------------------------*/

// get open price mid market
float Bar::getOpen() const { return .5*(openBid+openAsk); }

/*-------------------------------------------------------------------------------------------------*/

// get high price mid market
float Bar::getHigh() const { return .5*(highBid+highAsk); }

/*-------------------------------------------------------------------------------------------------*/

// get low price mid market
float Bar::getLow() const { return .5*(lowBid+lowAsk); }

/*-------------------------------------------------------------------------------------------------*/

// get close mid market
float Bar::getClose() const { return 0.5*(closeBid+closeAsk); }

/*-------------------------------------------------------------------------------------------------*/

// output bar
void Bar::print() const 
{
   std::cout << sec_to_string(date) << " ";
   std::cout << std::setw(8) << openBid << " ";
   std::cout << std::setw(8) << openAsk << " ";
   std::cout << std::setw(8) << highBid << " ";
   std::cout << std::setw(8) << highAsk << " ";
   std::cout << std::setw(8) << lowBid << " ";
   std::cout << std::setw(8) << lowAsk << " ";
   std::cout << std::setw(8) << closeBid << " ";
   std::cout << std::setw(8) << closeAsk << " ";
   std::cout << std::setw(7) << volume << "\n";
}

/*-------------------------------------------------------------------------------------------------*/

// cout overload for outputting a Bar
std::ostream& operator<<(std::ostream& out, Bar& x)
{
    x.print();

    return out;
}

//=================================================================================================

} 

#endif
