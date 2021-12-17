#ifndef BOOST_ASIO_SERVER_CONNECTION_H
#define BOOST_ASIO_SERVER_CONNECTION_H



#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>

namespace client {

	class Client : public std::enable_shared_from_this<Client>
	{
	public:
		explicit Client(boost::asio::io_context& io_context);

		void push(const std::string& server,
			const std::string& port,
			const std::string& message);


		void show_server(const std::string& server,
			const std::string& port);

		void download(const std::string& server,
                                const std::string& port,
                                const std::string& key,
                                const std::string& path);
	private:
		void handle_resolve(beast::error_code err,
			tcp::resolver::results_type results);

		void handle_connect(beast::error_code err,
			tcp::resolver::results_type::endpoint_type);

		void handle_write(beast::error_code err,
			std::size_t bytes_transferred);

		void handle_write_file(beast::error_code err,
			std::size_t bytes_transferred);

		void handle_read(beast::error_code err,
			std::size_t bytes_transferred);

		void handle_write_last(beast::error_code err,
			std::size_t bytes_transferred);

		void do_close();

	private:
		tcp::resolver resolver_;
		beast::tcp_stream stream_;
		beast::flat_buffer buffer_; // (Must persist between reads)
		beast::multi_buffer buffer_file; // (Must persist between reads)
		std::string client_path;

		http::request<http::file_body> request_file;
		http::request<http::string_body> request_;
		http::response<http::string_body> response_;
		http::response<http::file_body> response_file;
		std::shared_ptr<void> res_;
	};
}
#endif
