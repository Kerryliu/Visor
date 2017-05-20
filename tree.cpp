#include "tree.h"

Tree::Tree() : m_Dispatcher(), m_WorkerThread(nullptr) {
  // Find all devices:
  for (auto &p : fs::directory_iterator(file_path)) {
    devices.push_back(Device(p.path().string()));
  }
  // Get initial values:
  for (auto &device : devices) {
    device.refresh_sensors();
  }

  make_tree_view();

  // Start a new worker thread.
  m_Dispatcher.connect(sigc::mem_fun(*this, &Tree::update_tree_view));
  m_WorkerThread = new std::thread([this] { update_values(); });
}

Tree::~Tree() {
  delete m_Columns.sensor_column;
}

void Tree::make_tree_view() {
  m_refTreeModel = Gtk::TreeStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);
  Gtk::TreeModel::Row row;
  Gtk::TreeModel::Row child_row;
  Gtk::TreeModel::Row baby_child_row;

  for (unsigned int i = 0; i < devices.size(); i++) {
    // Get Readings
    vector<vector<Device::sensor_reading>> readings =
        devices[i].get_sensor_readings();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = devices[i].name + ": ";
    row[m_Columns.m_pixbuf] =
        Gdk::Pixbuf::create_from_file("assets/chip.svg", 20, 20, true);

    for (unsigned int sensor_type = 0; sensor_type < readings.size();
         sensor_type++) {
      if (!readings[sensor_type].empty()) {
        child_row = *(m_refTreeModel->append(row.children()));
        child_row[m_Columns.m_col_name] =
            devices[i].sensor_types[sensor_type] + ": ";
        child_row[m_Columns.m_pixbuf] = Gdk::Pixbuf::create_from_file(
            devices[i].sensor_types_icons[sensor_type], 20, 20, true);

        for (unsigned int j = 0; j < readings[sensor_type].size(); j++) {
          baby_child_row = *(m_refTreeModel->append(child_row.children()));
          baby_child_row[m_Columns.m_col_name] =
              readings[sensor_type][j].name + ": ";
          baby_child_row[m_Columns.m_pixbuf] =
              Gdk::Pixbuf::create_from_file("assets/sensor.svg", 20, 20, true);
          baby_child_row[m_Columns.m_col_current_value] = Device::formatValue(
              readings[sensor_type][j].current_value, sensor_type);
          baby_child_row[m_Columns.m_col_min_value] = Device::formatValue(
              readings[sensor_type][j].min_value, sensor_type);
          baby_child_row[m_Columns.m_col_max_value] = Device::formatValue(
              readings[sensor_type][j].max_value, sensor_type);
          baby_child_row[m_Columns.m_col_average_value] = Device::formatValue(
              readings[sensor_type][j].average_value, sensor_type);
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

  // Connect signal:
  m_TreeView.signal_row_activated().connect(
      sigc::mem_fun(*this, &Tree::on_treeview_row_activated));

  m_TreeView.expand_all();
  m_TreeView.set_enable_tree_lines(true);
  m_TreeView.set_rules_hint(true);
  m_TreeView.get_column(0)->set_expand(true);

  for (unsigned int i = 0; i < m_TreeView.get_n_columns(); i++) {
    m_TreeView.get_column(i)->set_resizable(true);
  }
}

void Tree::update_values() {
  while (1) {
    if (stop_work) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (auto &device : devices) {
      device.refresh_sensors();
    }
    m_Dispatcher.emit();
  }
}

void Tree::update_tree_view() {
  Gtk::TreeModel::Children rows = m_refTreeModel->children();
  unsigned int device_index = 0;
  for (auto &device_row : rows) {
    // Get Readings
    vector<vector<Device::sensor_reading>> readings =
        this->devices[device_index].get_sensor_readings();

    // Get Fan & Temp children of device
    Gtk::TreeModel::Children sensor_types_row = device_row->children();
    Gtk::TreeModel::Children::iterator iter_sensor_type =
        sensor_types_row.begin();

    for (unsigned int sensor_type = 0; sensor_type < readings.size();
         sensor_type++) {
      if (!readings[sensor_type].empty()) {
        Gtk::TreeModel::Children sensors = iter_sensor_type->children();
        Gtk::TreeModel::Children::iterator iter_sensors = sensors.begin();
        for (unsigned int j = 0; j < readings[sensor_type].size();
             j++, iter_sensors++) {
          Gtk::TreeModel::Row values = *iter_sensors;
          values[m_Columns.m_col_current_value] = Device::formatValue(
              readings[sensor_type][j].current_value, sensor_type);
          values[m_Columns.m_col_min_value] = Device::formatValue(
              readings[sensor_type][j].min_value, sensor_type);
          values[m_Columns.m_col_max_value] = Device::formatValue(
              readings[sensor_type][j].max_value, sensor_type);
          values[m_Columns.m_col_average_value] = Device::formatValue(
              readings[sensor_type][j].average_value, sensor_type);
        }
        iter_sensor_type++;
      }
    }
    device_index++;
  }
}

void Tree::on_treeview_row_activated(const Gtk::TreeModel::Path &path,
                                     Gtk::TreeViewColumn * /* column */) {
  Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    // row[m_Columns.m_col_name] = "poop";
    std::cout << "Row activated: ID = " << row[m_Columns.m_col_name]
              << ", Current Value = " << row[m_Columns.m_col_current_value]
              << std::endl;
  }
}

void Tree::on_quit() {
  if (m_WorkerThread) {
    // Order the worker thread to stop and wait for it to stop.
    stop_work = true;
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
  }
}