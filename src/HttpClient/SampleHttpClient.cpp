#include <exception>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include "Tester.h"
int main(){


  std::cout<<"Inside cpp file" << std::endl;

  boost::asio::io_context context;
  {
    auto work = boost::asio::make_work_guard(context);
    
    Tester tester(context);
    tester();


    std::thread th([&context]() { 
      try{
        context.run();
        std::cout<< "Io context is out of work"<< std::endl;
      }
      catch (const std::exception& ex){

        std::cout<< "exception occured. Message:" << ex.what() << std::endl;
      }
    });

    std::cout << "Press enter to stop the app" << std::endl;
    std::cin.get();

    work.reset();
    context.stop();

    th.join();
 }

  std::cout << "done." << std::endl;
  return 0;
}
