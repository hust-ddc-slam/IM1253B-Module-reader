
#include "IM1253B.h"
#include <linux/ioctl.h>


IM1253B::IM1253B (std::string dev):portName_(std::move(dev)), current_(0.0), voltage_(0.0)
{
    try{
        sp_ = new serial_port(ioSev_, portName_);
        sp_->set_option(serial_port::baud_rate(4800));
        sp_->set_option(serial_port::flow_control(serial_port::flow_control::none));
        sp_->set_option(serial_port::parity(serial_port::parity::none));
        sp_->set_option(serial_port::stop_bits(serial_port::stop_bits::one));
        sp_->set_option(serial_port::character_size(8));
    }catch (...){
        std::cerr << "Exception Error: Cannot open port: "<<portName_ <<", did run 'chmod' before?"<< std::endl;
    }
}

void IM1253B::start_asking_data(int frequency){
    frequency_ = frequency;
    std::thread t1(&IM1253B::ask_voltage_current, this);
    t1.detach();
    std::cout <<"Start asking for data all the time! " << std::endl;
}

void IM1253B::ask_voltage_current(void){
    while(true){
        std::cout <<"asking for data ... " << std::endl;
        try{
            uint8_t data[] = {0x01, 0x03, 0x00, 0x48, 0x00, 0x02, 0x44, 0x1D};  // header: 01,03, start: 00,48, register: 2, CRC: 44,1D
            boost::asio::write(*sp_, boost::asio::buffer(data, sizeof(data)));
        }
        catch(std::exception &e){
            std::cerr << "Exception Error: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/frequency_));
    }
}


// data: 01, 03, 08, xx,xx, yy,yy, CRC_low, CRC_high
bool IM1253B::read_voltage_current(void){
    unsigned char data[9];
    boost::asio::read(*sp_, boost::asio::buffer(data, 3));
    if(data[0] == 0x01 && data[1] == 0x03 && data[2] == 0x08){
        std::cout << "<-- data received. " << std::endl;
        boost::asio::read(*sp_, boost::asio::buffer(&data[3], 6));  // read following 6 bytes
        uint16_t crc = calculate_crc(data, 7);              // calculate 7 bytes CRC
        if(crc != static_cast<uint16_t>(data[8] << 8 | data[7])){
            std::cout << "CRC check failed. " << std::endl;
            return false;
        }
        voltage_ = (data[3] << 8 | data[4]) * 1e-4;       // voltage: 0.0001V 
        current_ = (data[5] << 8 | data[6]) * 1e-4;       // current: 0.0001A
        // std::cout << "voltage: " << voltage_ <<"V, current: " << current_ << "A" << std::endl;
    }
    return true;
}



uint16_t IM1253B::calculate_crc(const uint8_t *data, size_t len){
    uint16_t crc=0xFFFF;
    for(int i=0; i<len; ++i){
        crc ^= static_cast<uint16_t>(data[i]);
        for(int j=0; j<8; ++j){
            if(crc & 0x0001){
                crc >>= 1;
                crc ^= 0xA001;
            }else{
                crc >>= 1;
            }
        }
    }
    return crc;
}

