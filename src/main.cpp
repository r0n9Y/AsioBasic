#include "AsioConfig.h"
#include "NetMessage.h"


// From Damian Jarek
// `socket.is_open()` doesn't actually check if the connection is open. Instead, it checks whether we have a valid file descriptor (which on most systems boils down to `socket.native_handle() != -1`).
// - Creating a work guard just to prevent your `asio::io_context` from exiting is usually a code smell. The work guard is idiomatically used to tell ASIO that "there's an async operation in flight somewhere, that you don't see, but trust me, I'll tell you when it completes"
// - Synchronous ASIO functions offer 2 overloads, the one without an error code throws a `system_error` exception that contains the error code and the message, the overload with an error code parameter sets the error code. Make sure you actually call the overload you intended!
// - `socket.available()` is best avoided when dealing with TCP/IP streams, because your protocol needs to handle message fragmentation anyway (that's the nature of TCP/IP, in a pathological case, you should be ready to read 1 byte at a time). For simple protocols you can use the `asio::read` algorithms with a CompletionCondition that let you tell ASIO to read bytes until some condition is true (e.g. you read at least n bytes).

// And a few tips:
// - The last parameter to `async_` functions accepts a so-called CompletionToken. It's a fancy way of customizing ASIO-compatible async algorithms to tell it how the algorithm should start(eagerly or lazily) or how it should deliver its results (call a callback, set the result of a future, resume a coroutine or resume a fiber). While understanding this mechanism is definitely out of scope of this tutorial, it's worth at least knowing of the existence of this thing.
// - ASIO programs are simplest to write when your contexts are single-threaded. Remember, if you're doing asynchronous networking, you get quite far on just one thread!

std::vector<char> vBuffer(10 * 1024);

void FetchSome(asio::ip::tcp::socket& socket)
{
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](std::error_code ec, std::size_t size)
        {
            if(!ec)
            {
                std::cout << NL NL << "Read" << size << "bytes" << NL NL;
                for(int i = 0; i<size; ++i)
                {
                    std::cout << vBuffer[i];
                }
                FetchSome(socket);
            }
        });
}

int main(int argc, char **argv)
{
    net::Message<int> msg;
    std::cout << msg.header.id << NL;
    asio::error_code ec;

    asio::io_context ctx;
    asio::io_context::work idleTask(ctx);
    std::thread ctxThread = std::thread([&](){ ctx.run(); });

    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);
    asio::ip::tcp::socket socket(ctx);
    socket.connect(endpoint, ec);

    if(!ec){
        std::cout << "Connected ..." << NL;
    } else {
        std::cout << "Failed to connect " << ec.message() << NL;
    }

    // `socket.is_open()` doesn't actually check if the connection is open. Instead, it checks whether we have a valid file descriptor (which on most systems boils down to `socket.native_handle() != -1`).
    if(socket.is_open())
    {
        FetchSome(socket);
        std::string request = 
            "GET /index.html HTTP/1.1" CR
            "Host: example.com" CR
            "Connection: close" CR CR;

        socket.write_some(asio::buffer(request.data(), request.size()), ec);

        
        // auto bytes = socket.available();
        // std::cout << "Response: " << bytes << NL;

        // if(bytes) 
        // {
        //     std::vector<char> response(bytes);
        //     socket.read_some(asio::buffer(response.data(), response.size()), ec);
        //     for(auto character: response)
        //     {
        //         std::cout << character;
        //     }
        // }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);
    }
    else
    {}
    system("pause");
    return 0;
}