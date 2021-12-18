#include "connection.h"

#include <vector>
#include <boost/bind/bind.hpp>
#include <boost/beast/http/write.hpp>
#include <iostream>
#include <experimental/filesystem>
#include <string>
#include <sstream>
#include <fstream>

namespace server3 {
    
        std::stringstream ss;
        Connection::Connection(tcp::socket&& socket)
                : stream_(std::move(socket))
        {
        }

        void Connection::start()
        {
            net::dispatch(stream_.get_executor(),
                          beast::bind_front_handler(&Connection::do_read_first,
                                             shared_from_this()));

        }

        void Connection::do_read_first()
        {
            request_ = {};
            http::async_read(stream_, buffer_, request_,
                             beast::bind_front_handler(
                                     &Connection::handle_read_first,
                                     shared_from_this()));
           
            
        }


        void Connection::handle_read_first(beast::error_code e,
                                     std::size_t bytes_transferred)
        {     
            std::stringstream key_path;
            show_base(key_path);
            if (!e)
            {       if (request_.method()==http::verb::post)
                    {
                        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                        res.set(http::field::content_type, "application/text");
                        res.result(http::status::ok);
                        res.body() =  "file";
                        res.prepare_payload();

                        http::async_write(stream_,std::move(res), beast::bind_front_handler(
                                                            &Connection::handle_write_first,
                                                            shared_from_this()));
                    }
                    else{
                        if (request_.target()=="show")
                        {
                            std::stringstream key_path;
                            show_base(key_path);
                            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                            res.set(http::field::content_type, "application/text");
                            res.result(http::status::ok);
                            res.body() =key_path.str();
                            res.prepare_payload();

                             http::async_write(stream_,std::move(res), beast::bind_front_handler(
                                                            &Connection::handle_write_last,
                                                            shared_from_this()));

                        }else{
                                
                                beast::error_code ec;
                                http::file_body::value_type resp_body;
                                std::cout<<(local_base[request_.body()]).c_str()<<std::endl;
                                resp_body.open((local_base[request_.body()]).c_str(), beast::file_mode::read, ec);

                                // Handle the case where the file doesn't exist
                                if(ec == beast::errc::no_such_file_or_directory)
                                        std::cout << "Error: " << ec << "\n";

                                // Handle an unknown error
                                if(ec)
                                        std::cout << "Error: " << ec.message() << "\n";

                                // Cache the size since we need it after the move
                                auto const size = resp_body.size();

                                res_file.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                                res_file.set(http::field::content_type, "application/text");
                                res_file.body() =  std::move(resp_body);
                                res_file.content_length(resp_body.size());

                                http::async_write(stream_,res_file, beast::bind_front_handler(
                                                            &Connection::handle_write_last,
                                                            shared_from_this()));

                        }
                    }
            }else{
                
                std::cout << "Error: " << e.message() << "\n";
            }
                
;
        }


        void Connection::do_read()
        {
            std::filesystem::path path{server_path}; //creates TestingFolder object on C:
            path /=   "record"+ std::to_string(number_of_files(server_path))+".wav"; //put something into there
            std::filesystem::create_directories(path.parent_path()); //add directories based on the object path (without this line it will not work)

            std::ofstream ofs(path);
            ofs.close();

            beast::error_code ec;
            request_file.body().open(path.c_str(),boost::beast::file_mode::write, ec);

            if(ec == beast::errc::no_such_file_or_directory)
                std::cout << "Error: " << ec << "\n";

            // Handle an unknown error
            if(ec)
                std::cout << "Error: " << ec.message() << "\n";;

            http::async_read(stream_, buffer_File, request_file,
                             beast::bind_front_handler(
                                     &Connection::handle_write_last,
                                     shared_from_this()));

            // Read a request
            
        }

        void Connection::handle_write_first(beast::error_code e,
                                      std::size_t bytes_transferred)
        {   
            if (!e)
            {
                do_read();
            }
            else
            {           
                std::cout << "Error: " << e.message() << "\n";
                do_close();
            }
        }
        void Connection::handle_write_last(beast::error_code e,
                                      std::size_t bytes_transferred)
        {   
            if (!e)
            {
                do_close();
            }
            else
            {   std::cout << "Pampam " << "\n";
                do_close();      
                std::cout << "Error: " << e.message() << "\n";
            }
        }

        void Connection::do_close()
        {
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

        }

        void Connection::show_base(std::stringstream& key_path)
        {
            
            if (!std::filesystem::exists(server_path)) {
            std::cout << "No such directory" <<std::endl;
            }

            for (const auto & entry : std::filesystem::directory_iterator(server_path)) 
            {
                if (entry.path().extension() == ".wav") 
                {
                local_base[entry.path().stem()] = entry.path();
                key_path << " key :"<< entry.path().stem()<<" path: "<< entry.path()<<"\r\n";
                }
            }

        }

            std::size_t Connection::number_of_files(std::filesystem::path path)
        {
            using std::filesystem::directory_iterator;
            return std::distance(directory_iterator(path), directory_iterator{});
        }
     // namespace server3
}