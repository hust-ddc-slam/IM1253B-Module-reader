
#ifndef IM1253B_H_
#define IM1253B_H_

#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>


using boost::asio::serial_port;
class IM1253B{
public:
    IM1253B() = default;
    explicit IM1253B(std::string dev); // construct func.
    void start_asking_data(int frequency);
    bool read_voltage_current(void);   // read IM1253's reply;
    double current_, voltage_;

private:

    void ask_voltage_current(void);    // ask for IM1253's reply;
    boost::asio::serial_port *sp_{};
    boost::asio::io_service ioSev_;
    std::string portName_;
    boost::system::error_code err_;
    int frequency_;
    uint16_t calculate_crc(const uint8_t *data, size_t len);
};

#endif
