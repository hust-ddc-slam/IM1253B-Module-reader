
#include "IM1253B.h"
#include <iostream>
#include <boost/asio.hpp>

using namespace std;


int main(void){
    string port_name = "/dev/ttyTHS0";
    cout << "==> Open port: " << port_name << endl;
    IM1253B mo(port_name);
    
    int frequency = 2;
    mo.start_asking_data(frequency);

    cout << "Begin to receive data..." << endl;
    while(1){
        if(mo.read_voltage_current()){
            cout << "voltage: " << mo.voltage_ << "V, current: " << mo.current_ << "A" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(10));   // sleep 10ms
    }
    return 0;
}
