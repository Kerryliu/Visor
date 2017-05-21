#include "tree.h"
#include <experimental/filesystem>
#include <gtkmm.h>
#include <iostream>
#include <vector>

namespace fs = std::experimental::filesystem;

Tree::Tree(vector<vector<vector<Device::sensor_reading>>> &all_readings,
           vector<string> &device_names) {
  this->all_readings = all_readings;
  this->device_names = device_names;

  make_tree_view();
}

Tree::~Tree() { delete m_Columns.sensor_column; }

void Tree::make_tree_view() {
  m_refTreeModel = Gtk::TreeStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);
  Gtk::TreeModel::Row device_row;
  Gtk::TreeModel::Row sensor_types_row;
  Gtk::TreeModel::Row readings_row;

  for (unsigned int i = 0; i < all_readings.size(); i++) {
    vector<vector<Device::sensor_reading>> device_readings = all_readings[i];

    device_row = *(m_refTreeModel->append());
    device_row[m_Columns.m_col_name] = device_names[i] + ": ";
    device_row[m_Columns.m_pixbuf] =
        Gdk::Pixbuf::create_from_file("assets/chip.svg", 20, 20, true);

    for (unsigned int sensor_type = 0; sensor_type < device_readings.size();
         sensor_type++) {
      if (!device_readings[sensor_type].empty()) {
        sensor_types_row = *(m_refTreeModel->append(device_row.children()));
        sensor_types_row[m_Columns.m_col_name] =
            Device::sensor_types[sensor_type] + ": ";
        sensor_types_row[m_Columns.m_pixbuf] = Gdk::Pixbuf::create_from_file(
            Device::sensor_types_icons[sensor_type], 20, 20, true);

        for (unsigned int j = 0; j < device_readings[sensor_type].size(); j++) {
          readings_row = *(m_refTreeModel->append(sensor_types_row.children()));
          readings_row[m_Columns.m_col_name] =
              device_readings[sensor_type][j].name + ": ";
          readings_row[m_Columns.m_pixbuf] =
              Gdk::Pixbuf::create_from_file("assets/sensor.svg", 20, 20, true);
          readings_row[m_Columns.m_col_current_value] = Device::formatValue(
              device_readings[sensor_type][j].current_value, sensor_type);
          readings_row[m_Columns.m_col_min_value] = Device::formatValue(
              device_readings[sensor_type][j].min_value, sensor_type);
          readings_row[m_Columns.m_col_max_value] = Device::formatValue(
              device_readings[sensor_type][j].max_value, sensor_type);
          readings_row[m_Columns.m_col_average_value] = Device::formatValue(
              device_readings[sensor_type][j].average_value, sensor_type);
        }
      }
    }
  }

  // Add the TreeView's view columns:
  m_Columns.sensor_column->pack_start(m_Columns.m_pixbuf, false);
  m_Columns.sensor_column->pack_end(m_Columns.m_col_name, true);
  m_TreeView.append_column(*m_Columns.sensor_column);
  m_TreeView.append_column("Current", m_Columns.m_col_current_value);
  m_TreeView.append_column("Min", m_Columns.m_col_min_value);
  m_TreeView.append_column("Max", m_Columns.m_col_max_value);
  m_TreeView.append_column("Average", m_Columns.m_col_average_value);

  m_TreeView.expand_all();
  m_TreeView.set_enable_tree_lines(true);
  m_TreeView.set_rules_hint(true);
  m_TreeView.get_column(0)->set_expand(true);

  for (unsigned int i = 0; i < m_TreeView.get_n_columns(); i++) {
    m_TreeView.get_column(i)->set_resizable(true);
  }
}

void Tree::update_tree_view(
    vector<vector<vector<Device::sensor_reading>>> &all_readings) {
  this->all_readings = all_readings;
  Gtk::TreeModel::Children rows = m_refTreeModel->children();
  unsigned int device_index = 0;
  for (auto &device_row : rows) {
    // Get Readings
    vector<vector<Device::sensor_reading>> device_readings =
        all_readings[device_index];

    // Get Fan & Temp children of device
    Gtk::TreeModel::Children sensor_types_row = device_row->children();
    Gtk::TreeModel::Children::iterator iter_sensor_type =
        sensor_types_row.begin();

    for (unsigned int sensor_type = 0; sensor_type < device_readings.size();
         sensor_type++) {
      if (!device_readings[sensor_type].empty()) {
        Gtk::TreeModel::Children sensors = iter_sensor_type->children();
        Gtk::TreeModel::Children::iterator iter_sensors = sensors.begin();
        for (unsigned int j = 0; j < device_readings[sensor_type].size();
             j++, iter_sensors++) {
          Gtk::TreeModel::Row values = *iter_sensors;
          values[m_Columns.m_col_current_value] = Device::formatValue(
              device_readings[sensor_type][j].current_value, sensor_type);
          values[m_Columns.m_col_min_value] = Device::formatValue(
              device_readings[sensor_type][j].min_value, sensor_type);
          values[m_Columns.m_col_max_value] = Device::formatValue(
              device_readings[sensor_type][j].max_value, sensor_type);
          values[m_Columns.m_col_average_value] = Device::formatValue(
              device_readings[sensor_type][j].average_value, sensor_type);
        }
        iter_sensor_type++;
      }
    }
    device_index++;
  }
}
