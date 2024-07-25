# ncmqtt

`ncmqtt` is a command-line tool for sharing data over MQTT.

## Authors

- Manuele Conti ([manuele.conti@archimede-energia.com](mailto:manuele.conti@archimede-energia.com))
- Alessio Antoniazza ([alessio.antoniazza@archimede-energia.com](mailto:alessio.antoniazza@archimede-energia.com))

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

## Disclaimer

This code is made available on the understanding that it will not be used in safety-critical situations without a full and competent review.

## Requirements

- [CMake](https://cmake.org/) 3.16 or higher
- C compiler (e.g., gcc)
- [Eclipse Paho MQTT C client library](https://www.eclipse.org/paho/)

## Building

To build the `ncmqtt` project, follow these steps:

1. **Clone the repository**:
    ```sh
    git clone <repository_URL>
    cd ncmqtt
    ```

2. **Set the Path to Paho MQTT Library**:

    - You can set the path to the Paho MQTT library using the `PAHO_MQTT_DIR` variable when configuring the project with CMake.
    - If `PAHO_MQTT_DIR` is not set, CMake will attempt to find the Paho MQTT library using the `find_package` command.


### Build the Project

#### A. Build with `PAHO_MQTT_DIR` set:

1. Create and navigate to the `build` directory:
   ```sh
   mkdir build
   cd build
   ```
2. Run `cmake` with the `PAHO_MQTT_DIR` set to the path of your Paho MQTT C library:
   ```sh
   cmake -DPAHO_MQTT_DIR=/path/to/paho.mqtt.c ..
   ```
3. Build the project:
   ```sh
   make
   ```

#### B. Build without `PAHO_MQTT_DIR` set:

1. Create and navigate to the `build` directory:
   ```sh
   mkdir build
   cd build
   ```
2. Run `cmake` without setting the `PAHO_MQTT_DIR`:
   ```sh
   cmake ..
   ```
3. Build the project:
   ```sh
   make
   ```

## Usage

The `ncmqtt` command supports various options to configure its behavior:

```sh
Usage: ncmqtt [<options>]
 -h : show this help message
 -v : show version
 -T : topic for sharing messages: default = nc_channel_pub
 -l : enable listening to incoming messages
 -S : set server host : default = test.mosquitto.org
 -P : set server port : default = 1883
 -A : set server auth = user,pwd

