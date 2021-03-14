#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost_tcp = boost::asio::ip::tcp;

int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "Host and service must be provided\n";
    return EXIT_FAILURE;
  }

  auto conn = [&]() {
    try {
      std::string host = argv[1];
      std::string service = argv[2];

      boost::asio::io_service io_service;
      boost_tcp::resolver resolver(io_service);

      boost_tcp::resolver::query query(host, service);
      boost_tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
//        resolver.resolve(argv[1], "daytime");

      boost_tcp::socket socket(io_service);
      boost::asio::connect(socket, endpoint_iterator);
      std::cout << "Connected to " << host << " " << service << std::endl;

      for (;;) {
        boost::array<char, 128> buf{};
        boost::system::error_code error;

        std::size_t len = socket.receive(boost::asio::buffer(buf));

        if (error == boost::asio::error::eof)
          break;
        else if (error)
          throw boost::system::system_error(error);

        std::cout.write(buf.data(), len);
      }

    }
    catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  };

  std::vector<std::thread> threads;

  for(int i=0; i<10; ++i)
    threads.emplace_back(conn);


  for(auto& thread : threads)
    thread.join();

  return EXIT_SUCCESS;
}