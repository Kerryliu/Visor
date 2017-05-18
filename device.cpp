#include "device.h"

Device::Device(string file_path)
    : file_path(file_path), name(set_name(file_path)),
      sensor_type_counts(set_sensor_count(file_path)) {
  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    sensor_readings[type].resize(sensor_type_counts[type]);
  }
}

const vector<vector<std::pair<string, int>>> &
Device::get_sensor_readings() {
  return sensor_readings;
}

string Device::set_name(string file_path) {
  std::ifstream file;
  string file_name;
  file.open(file_path + '/' + "name");
  getline(file, file_name);
  file.close();
  return file_name;
}

const vector<unsigned int>
Device::set_sensor_count(string file_path) {
  vector<unsigned int> sensor_type_counts(sensor_types.size());
  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    std::ifstream file;
    for (unsigned int sensor_count = 0; sensor_count < 100; sensor_count++) {
      file.open(file_path + sensor_types[type] +
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

void Device::refresh_sensors() {
  std::ifstream file;
  string sensor_name;
  int sensor_value;

  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    vector<std::pair<string, int>> sensor_type_readings(
        sensor_type_counts[type]);

    for (unsigned int sensor_number = 0;
         sensor_number < sensor_type_counts[type]; sensor_number++) {
      // Get sensor value:
      file.open(file_path + sensor_types[type] +
                std::to_string(sensor_number + 1) + "_input");
      string temp_string;
      getline(file, temp_string);
      sensor_value = std::stoi(temp_string);
      file.close();

      // see if sensor value has name:
      file.open(file_path + sensor_types[type] +
                std::to_string(sensor_number + 1) + "_label");
      if (file) {
        getline(file, sensor_name);
        file.close();
      } else {
        string temp_string = sensor_types[type].substr(1);
        temp_string[0] = toupper(temp_string[0]);
        sensor_name =
            temp_string + " sensor #" + std::to_string(sensor_number + 1);
      }
      std::pair<string, int> sensor_reading(sensor_name, sensor_value);
      sensor_type_readings[sensor_number] = sensor_reading;
    }
    sensor_readings[type] = sensor_type_readings;
  }
}
