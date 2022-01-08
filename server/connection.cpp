#include "connection.h"

#include <vector>
#include <boost/bind/bind.hpp>
#include <boost/beast/http/write.hpp>
#include <iostream>
#include <experimental/filesystem>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <time.h>

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
            show_base(key_path,server_path);
            if (!e)
            {       if (request_.method()==http::verb::post)
                    {
                        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                        res.set(http::field::content_type, "application/text");
                        res.result(http::status::ok);
                        res.body() =  "file";
                        res.prepare_payload();
                        s = std::move(request_.target());
                        http::async_write(stream_,std::move(res), beast::bind_front_handler(
                                                            &Connection::handle_write_first,
                                                            shared_from_this()));
                    }
                    else{
                        if (request_.target()=="show")
                        {
                            std::stringstream key_path;
                            show_base(key_path,server_path);
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
                                //std::cout<<(local_base[request_.body()]).c_str()<<std::endl;
                                std::string path_file = server_path +"/"+request_.body()+"/"+request_.body()+std::to_string(count_file)+".wav";
                                std::cout<<path_file<<std::endl;
                                std::cout<<server_path +"/"+request_.body()<<std::endl;
                                std::cout<<server_path +"/"+request_.body()+"/"+request_.body()+std::to_string(count_file)+".wav"<<std::endl;
                                resp_body.open(path_file.c_str(), beast::file_mode::read, ec);
                                count_file++;
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
                                std::cout<<"body size "<<resp_body.size()<<std::endl;
                                std::cout<<count_file<<std::endl;
                                std::cout<<server_path +"/"+std::string(s)<<std::endl;
                                std::cout<<number_of_files(server_path +"/"+request_.body())<<std::endl;

                                if (number_of_files(server_path +"/"+request_.body())- count_file == 0){
                                http::async_write(stream_,res_file, beast::bind_front_handler(
                                                            &Connection::handle_write_last,
                                                            shared_from_this()));
                                    count_file = 0 ;
                                }
                                else{
                                    std::cout<<"put in to client"<<std::endl;
                                    std::cout<<"fie number "<<count_file<<std::endl;
                                    http::async_write(stream_,res_file, beast::bind_front_handler(
                                                            &Connection::handle_write_last,
                                                            shared_from_this()));
                                } 

                        }
                    }
            }else{
                
                std::cout << "Error: " << e.message() << "\n";
            }
                
;
        }

        
        void Connection::do_read()
        {
            if (!std::filesystem::exists(server_path  +"/"+std::string(s))) {
                std::filesystem::create_directories(server_path  +"/"+std::string(s));
            }
           

            std::filesystem::path path{server_path +"/"+std::string(s)}; //creates TestingFolder object on C:
            //std::to_string(number_of_files(server_path))
            path /= std::string(s) + std::to_string(number_of_files(server_path +"/"+std::string(s)))+".wav"; //put something into there
            std::filesystem::create_directories(path.parent_path()); //add directories based on the object path (without this line it will not work)

            std::ofstream ofs(path);
            ofs.close();

            beast::error_code ec;
            request_file.body().open(path.c_str(),boost::beast::file_mode::write, ec);
            flag = 1;
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
        void Connection::read_requiest(beast::error_code e,
                                      std::size_t bytes_transferred)
        {
            http::async_read(stream_, buffer_, request_,
                             beast::bind_front_handler(
                                     &Connection::handle_read_first,
                                     shared_from_this()));
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

            if (flag){
                    if (request_file.body().size()!=0)
                    do_read();   
                    else
                    do_close();
            }
            else{
                if (!e){
                        do_close();
                }

                else
                {   std::cout << "Pampam " << "\n";
                    do_close();      
                    std::cout << "Error: " << e.message() << "\n";
                }
            }
        }

        void Connection::do_close()
        {
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

        }

        void Connection::show_base(std::stringstream& key_path,std::string& server_path)
        {
            
            if (!std::filesystem::exists(server_path)) {
            std::cout << "No such directory" <<std::endl;
            }

            for (const auto & entry : std::filesystem::directory_iterator(server_path)) 
            {
                local_base[entry.path().stem()] = entry.path();
            }
            for (auto it = local_base.begin(); it!=local_base.end(); it++) {
                struct stat t_stat;
                stat(it->second.c_str(), &t_stat);
                struct tm *timeinfo = localtime(&t_stat.st_ctime);
                key_path<<"file: "<< it->first <<"\t"<< "time: " << asctime(timeinfo);

            }
        }
            std::size_t Connection::number_of_files(std::filesystem::path path)
        {
            using std::filesystem::directory_iterator;
            return std::distance(directory_iterator(path), directory_iterator{});
        }
     // namespace server3
}


