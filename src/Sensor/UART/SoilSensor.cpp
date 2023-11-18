/* Sensor data format:

Soil Senor 3001-TR

	Interrogation frame

		Device address - 1 byte
			(1) [hex:0x01]
		Function code - 1 byte
			(2) [hex:0x03]
		Register address - 2 bytes
			(3,4)
		Register length - 2 bytes
			(5,6)
		Check code low bit - 1 byte
			(7)
		Check code high bit - 1 byte
			(8)

	Reply frame

		Device address - 1 byte
			(1) [hex:0x01]
		Function code - 1 byte
			(2) [hex:0x03]
		Quantity of valid byte - 1 byte
			(3) [hex:0x02]
		Data 1 Area - 2 byte
			(4,5)
		Data 2 Area - 2 byte
			(6,7)
		Check code
			
*/

#include <libserial/SerialPort.h>
#include <iostream>
#include <vector>

/* Interrogation frame */

// Default address of the sensor
const uint8_t SENSOR_ADDR = 0x01;
// Function code for defining action to be performed by the sensor
const uint8_t FUNC_CODE_READ = 0x03;
// Register address for aquirering data from soil sensor
const uint16_t ADDR_PH = 0x0006;               // pH value (Unit: 0.01 pH)
const uint16_t ADDR_MOISTURE = 0x0012;         // Humidity level (Unit 0.1% RH)
const uint16_t ADDR_TEMP = 0x0013;             // Tempreture (Unit: 0.1 C)
const uint16_t ADDR_CONDUCTIVITY = 0x0015;     // Conductivity (Unit: us/cm)
const uint16_t ADDR_NITROGEN = 0x001E;         // Nitrogen level (Unit: mg/kg)
const uint16_t ADDR_PHOSPHORUS = 0x001F;       // Phosphorus level (Unit: mg/kg) 
const uint16_t ADDR_POTASSIUM = 0x0020;        // Pottasium level (Unit: mg/kg)
const uint16_t ADDR_GET_SENSOR_ADDR = 0x0100;  // Address for reading sensor address (Range: 0~255)
const uint16_t ADDR_BAUD_RATE = 0x0101;        // R/W for BAUD rate (Range: 2400, 4800, 9600)

/* Reply frame */

uint16_t rx_data_ph;               // pH value (Unit: 0.01 pH)
uint16_t rx_data_moisture;         // Humidity level (Unit 0.1% RH)
uint16_t rx_data_temp;             // Tempreture (Unit: 0.1 C)
uint16_t rx_data_conductivity;     // Conductivity (Unit: us/cm)
uint16_t rx_data_nitrogen;         // Nitrogen level (Unit: mg/kg)
uint16_t rx_data_phosphorus;       // Phosphorus level (Unit: mg/kg) 
uint16_t rx_data_potassium;        // Pottasium level (Unit: mg/kg)




using namespace LibSerial;
using namespace std;

// Function to initialize and configure the serial port
void initializeSerialPort(SerialPort& serial_port, const string& port_name) {
    // Open the serial port
    serial_port.Open(port_name);

    // Set the baud rate
    serial_port.SetBaudRate(SerialPort::BAUD_9600);

    // Set the number of data bits
    serial_port.SetCharacterSize(SerialPort::CHAR_SIZE_8);

    // Set the parity
    serial_port.SetParity(SerialPort::PARITY_NONE);

    // Set the number of stop bits
    serial_port.SetStopBits(SerialPort::STOP_BITS_1);

    // Set hardware flow control
    serial_port.SetFlowControl(SerialPort::FLOW_CONTROL_NONE);
}

// Function to send a Modbus request
void sendModbusRequest(SerialPort& serial_port, const vector<uint8_t>& request) {
    serial_port.Write(request);
}

// Function to receive a Modbus response
vector<uint8_t> receiveModbusResponse(SerialPort& serial_port) {
    vector<uint8_t> response;
    serial_port.Read(response, 7, 1000); // Read 7 bytes with a timeout
    return response;
}

//
uint16_t calculateCRC(uint8_t* buffer, size_t length) {
    uint16_t crc = 0xFFFF; // Initialize CRC to 0xFFFF
    for (size_t i = 0; i < length; i++) {
        crc ^= buffer[i]; // XOR byte into least sig. byte of crc
        for (uint8_t j = 8; j != 0; j--) { // Loop over each bit
            if ((crc & 0x0001) != 0) { // If the LSB is set
                crc >>= 1; // Shift right and XOR with polynomial
                crc ^= 0xA001;
            }
            else { // Else LSB is not set
                crc >>= 1; // Just shift right
            }
        }
    }
    // Note, this CRC is little-endian and may need to be reversed for transmission
    return crc;
}

// Main function
int main() {
    try {
        // Initialize the serial port
        SerialPort serial_port;
        initializeSerialPort(serial_port, "/dev/ttyTHS2"); // Replace with your actual serial port

        // Create a Modbus request frame (example for reading soil temperature and humidity)
        vector<uint8_t> request = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B }; // Replace with actual request frame

        // Send the request
        sendModbusRequest(serial_port, request);

        // Receive the response
        vector<uint8_t> response = receiveModbusResponse(serial_port);

        // Process the response (implement based on the Modbus protocol)
        // ...

        // Close the serial port
        serial_port.Close();
    }
    catch (const OpenFailed&) {
        cerr << "The serial port did not open correctly." << endl;
    }
    catch (const ReadTimeout&) {
        cerr << "A read timeout occurred" << endl;
    }

    return 0;
}
