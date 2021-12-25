#include "async.http.client.h"

#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#define BOOST_NO_SCOPED_ENUMS 
#include "boost/filesystem.hpp" 
#undef BOOST_NO_SCOPED_ENUMS

#include <boost/date_time/posix_time/posix_time.hpp>

namespace client {

    Client::Client(boost::asio::io_context& io_context)
        : resolver_(net::make_strand(io_context)),
        stream_(net::make_strand(io_context))
    {
    }

    void Client::push(const std::string& server,
        const std::string& port,
        const std::string& message,
        const std::string& path)
    {
        /// send server request if it chat  
        request_.version(10);
        request_.method(http::verb::post);
        request_.target(message);
        request_.set(http::field::host, server);
        request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_.prepare_payload();

        /// send server request if it file
        beast::error_code ec;
        http::file_body::value_type body;
        body.open(path.c_str(), beast::file_mode::read, ec);

        // Handle the case where the file doesn't exist
        if (ec == beast::errc::no_such_file_or_directory)
            std::cout << "Error: " << ec << "\n";

        // Handle an unknown error
        if (ec)
            std::cout << "Error: " << ec.message() << "\n";;

        // Cache the size since we need it after the move
        // use the full host:port here
        request_file.method(http::verb::post);
        request_file.version(10);
        request_file.target(message);
        request_file.content_length(body.size());
        request_file.set(http::field::host, server);
        request_file.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_file.body() = std::move(body);
        request_file.prepare_payload();


        resolver_.async_resolve(server, port,
            beast::bind_front_handler(
                &Client::handle_resolve,
                shared_from_this()));
    }

    void Client::show_server(const std::string& server,
        const std::string& port)
    {
        request_.version(10);
        request_.method(http::verb::get);
        request_.target("show");
        request_.set(http::field::host, server);
        request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_.prepare_payload();

        resolver_.async_resolve(server, port,
            beast::bind_front_handler(
                &Client::handle_resolve,
                shared_from_this()));
    }

    void Client::download(const std::string& server,
                                const std::string& port,
                                const std::string& key,
                                const std::string& path)
    {
        client_path = path;
        //boost::filesystem::path p(message.c_str());
        //check = "check";
        request_.version(10);
        request_.method(http::verb::get);
        request_.target("play");
        request_.body() = key;
        request_.set(http::field::host, server);
        request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request_.prepare_payload();

        resolver_.async_resolve(server, port,
            beast::bind_front_handler(
                &Client::handle_resolve,
                shared_from_this()));
    }

    void Client::handle_resolve(beast::error_code err,
        tcp::resolver::results_type results)
    {
        if (!err)
        {
            stream_.async_connect(results,
                        beast::bind_front_handler(
                            &Client::handle_connect,
                            shared_from_this()));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void Client::handle_connect(beast::error_code err, tcp::resolver::results_type::endpoint_type)
    {
        if (!err)
        {
            if (request_.target() == "play") {
                http::async_write(stream_, request_,
                    beast::bind_front_handler(
                        &Client::handle_write_file,
                        shared_from_this()));
            }
            else {
                http::async_write(stream_, request_,
                    beast::bind_front_handler(
                        &Client::handle_write,
                        shared_from_this()));
            }
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void Client::handle_write(beast::error_code err,
        std::size_t bytes_transferred)
    {
        if (!err)
        {
            http::async_read(stream_, buffer_, response_,
                beast::bind_front_handler(
                    &Client::handle_read,
                    shared_from_this()));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void Client::handle_write_file(beast::error_code err,
        std::size_t bytes_transferred)
    {
        if (!err)
        {
            //std::cout<<client_path<<std::endl;
            beast::error_code ecdd;
            
            // need create file for file from server
            //size_before = response_file.body().size();
            //std::cout<<"size_before bef" <<size_before<<std::endl;
            response_file.body().open(client_path.c_str(), boost::beast::file_mode::write, ecdd);
            // Handle the case where the file doesn't exist
            if (ecdd == beast::errc::no_such_file_or_directory)
                std::cout << "Error: " << ecdd << "\n";

            // Handle an unknown error
            if (ecdd)
                std::cout << "Error: " << ecdd.message() << "\n";

            http::async_read(stream_, buffer_, response_file,
                                        beast::bind_front_handler(
                                            &Client::handle_read,
                                            shared_from_this()));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
            do_close();
        }
    }

    void Client::handle_read(beast::error_code err,
        std::size_t bytes_transferred)
    {
        if (!err)
        {
            if (response_.body().size() > 4) {
                std::cout << response_.body() << std::endl;
            }

            if (response_.body() == "file") {
                http::async_write(stream_, request_file,
                    beast::bind_front_handler(
                        &Client::handle_write_last,
                        shared_from_this()));
            }

        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
            do_close();

        }
    }
    

    void Client::handle_write_last(beast::error_code err,
        std::size_t bytes_transferred)
    {
        do_close();
    }

    void Client::do_close()
    {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }

};


