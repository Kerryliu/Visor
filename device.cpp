#include "device.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using std::vector;
using std::string;

const vector<string> Device::sensor_types({"Voltage", "Fan", "PWM",
                                           "Temperature"});
const vector<string> Device::sensor_types_paths({"/in", "/fan", "/pwm",
                                                 "/temp"});
const vector<string> Device::sensor_types_icons(
    {"assets/in.svg", "assets/fan.svg", "assets/pwm.svg", "assets/temp.svg"});

Device::Device(string file_path)
    : file_path(file_path), name(set_name(file_path)),
      sensor_type_counts(set_sensor_count(file_path)) {
  // Resize vector that stores all the stuffs for the detected sensor count~
  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    sensor_readings[type].resize(sensor_type_counts[type]);
  }
}

const vector<vector<Device::sensor_reading>> &Device::get_sensor_readings() {
  return sensor_readings;
}

void Device::refresh_sensors() {
  vector<vector<Device::sensor_reading>> previous_sensor_readings =
      get_sensor_readings();
  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    vector<sensor_reading> current_sensor_type_readings(
        sensor_type_counts[type]);
    vector<sensor_reading> previous_sensor_type_readings =
        previous_sensor_readings[type];

    for (unsigned int sensor_number = 0;
         sensor_number < sensor_type_counts[type]; sensor_number++) {
      string current_sensor_name;

      // Previous values to compare against:
      sensor_reading previous_sensor_type_reading =
          previous_sensor_type_readings[sensor_number];
      // int previous_current_value =
      // previous_sensor_type_reading.current_value;
      int previous_min_value = previous_sensor_type_reading.min_value;
      int previous_max_value = previous_sensor_type_reading.max_value;
      int previous_average_sensor_value =
          previous_sensor_type_reading.average_value;
      int previous_refresh_count = previous_sensor_type_reading.refresh_count;

      // New values to be:
      int current_current_value;
      int current_min_value;
      int current_max_value;
      int current_average_value;
      int current_refresh_count;
      std::ifstream file;
      string temp_string;

      // Get sensor value:
      file.open(file_path + sensor_types_paths[type] +
                std::to_string(sensor_number + 1) + "_input");
      getline(file, temp_string);
      file.close();

      // Do calculations for min, max, and average
      current_current_value = std::stoi(temp_string);
      current_min_value = (current_current_value < previous_min_value)
                              ? current_current_value
                              : previous_min_value;
      current_max_value = (current_current_value > previous_max_value)
                              ? current_current_value
                              : previous_max_value;
      current_refresh_count = previous_refresh_count + 1;
      // Probably a better way of calculating average.  This will do for now.
      current_average_value =
          ((previous_average_sensor_value * previous_refresh_count) +
           current_current_value) /
          current_refresh_count;

      // see if sensor value has name:
      file.open(file_path + sensor_types_paths[type] +
                std::to_string(sensor_number + 1) + "_label");
      if (file) {
        getline(file, current_sensor_name);
        file.close();
      } else {
        current_sensor_name = "Sensor #" + std::to_string(sensor_number + 1);
      }

      // Create new sensor_reading struct for current values
      sensor_reading current_sensor_reading;
      current_sensor_reading.name = current_sensor_name;
      current_sensor_reading.current_value = current_current_value;
      current_sensor_reading.min_value = current_min_value;
      current_sensor_reading.max_value = current_max_value;
      current_sensor_reading.refresh_count = current_refresh_count;
      current_sensor_reading.average_value = current_average_value;
      current_sensor_type_readings[sensor_number] = current_sensor_reading;
    }
    sensor_readings[type] = current_sensor_type_readings;
  }
}

string Device::formatValue(int value, int sensor_type) {
  std::ostringstream temp;
  string value_string;
  switch (sensor_type) {
  case VOLTAGE:
    temp << std::setprecision(2) << std::fixed << ((double)value / 1000);
    value_string = temp.str();
    value_string += " V";
    return value_string;
  case TEMPERATURE:
    temp << std::setprecision(1) << std::fixed << ((double)value / 1000);
    value_string = temp.str();
    value_string += " \u2103";
    return value_string;
  default:
    value_string = std::to_string(value);
    value_string += " RPM";
    return value_string;
  }
}

string Device::set_name(string file_path) {
  std::ifstream file;
  string file_name;
  file.open(file_path + '/' + "name");
  getline(file, file_name);
  file.close();
  return file_name;
}

const vector<unsigned int> Device::set_sensor_count(string file_path) {
  vector<unsigned int> sensor_type_counts(sensor_types.size());
  // Try opening sensor file; if it succeeds, the sensor exists
  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    std::ifstream file;
    for (unsigned int sensor_count = 0; sensor_count < 100; sensor_count++) {
      file.open(file_path + sensor_types_paths[type] +
                std::to_string(sensor_count + 1) + "_input");
      if (file) {
        file.close();
      } else {
        sensor_type_counts[type] = sensor_count;
        break;
      }
    }
  }
  return sensor_type_counts;
}
