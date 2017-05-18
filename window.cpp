#include "window.h"

namespace fs = std::experimental::filesystem;

Window::Window()
    : m_Dispatcher(), m_WorkerThread(nullptr),
      m_VBox(Gtk::ORIENTATION_VERTICAL), m_Button_Quit("Quit") {
  set_title("Visor");
  set_border_width(1);
  set_default_size(800, 600);

  add(m_VBox);

  // Add the TreeView, inside a ScrolledWindow, with the button underneath:
  m_ScrolledWindow.add(m_TreeView);

  // Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_VBox.pack_start(m_ScrolledWindow);
  // m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);
  //
  // m_ButtonBox.pack_start(m_Button_Quit, Gtk::PACK_SHRINK);
  // m_ButtonBox.set_border_width(5);
  // m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
  // m_Button_Quit.signal_clicked().connect(
  //     sigc::mem_fun(*this, &Window::on_button_quit));

  // Find all devices
  for (auto &p : fs::directory_iterator(file_path)) {
    devices.push_back(Device(p.path().string()));
  }
  // Get initial values
  for (auto &device : devices) {
    device.refresh_sensors();
  }

  make_tree_view();

  show_all_children();
}

Window::~Window() {}

void Window::make_tree_view() {
  m_refTreeModel = Gtk::TreeStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);

  Gtk::TreeModel::Row row;
  Gtk::TreeModel::Row child_row;
  Gtk::TreeModel::Row baby_child_row;

  for (unsigned int i = 0; i < devices.size(); i++) {
    // Get Readings
    vector<vector<std::pair<std::string, int>>> readings =
        devices[i].get_sensor_readings();

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = devices[i].name;
    row[m_Columns.m_col_value] = "";

    // Temperature
    if (!readings[TEMPERATURE].empty()) {
      child_row = *(m_refTreeModel->append(row.children()));
      child_row[m_Columns.m_col_name] = "Temperature: ";
      child_row[m_Columns.m_col_value] = "";
      for (unsigned int j = 0; j < readings[TEMPERATURE].size(); j++) {
        baby_child_row = *(m_refTreeModel->append(child_row.children()));
        baby_child_row[m_Columns.m_col_name] =
            readings[TEMPERATURE][j].first + ": ";
        baby_child_row[m_Columns.m_col_value] =
            std::to_string(readings[TEMPERATURE][j].second / 1000);
      }
    }

    // Fan
    if (!readings[FAN].empty()) {
      child_row = *(m_refTreeModel->append(row.children()));
      child_row[m_Columns.m_col_name] = "Fan: ";
      child_row[m_Columns.m_col_value] = "";
      for (unsigned int j = 0; j < readings[FAN].size(); j++) {
        baby_child_row = *(m_refTreeModel->append(child_row.children()));
        baby_child_row[m_Columns.m_col_name] = readings[FAN][j].first + ": ";
        baby_child_row[m_Columns.m_col_value] =
            std::to_string(readings[FAN][j].second);
      }
    }
  }

  // Add the TreeView's view columns:
  m_TreeView.append_column("Name", m_Columns.m_col_name);
  m_TreeView.append_column("Value", m_Columns.m_col_value);

  // Connect signal:
  m_TreeView.signal_row_activated().connect(
      sigc::mem_fun(*this, &Window::on_treeview_row_activated));

  m_TreeView.expand_all();
  m_TreeView.set_enable_tree_lines(true);

  // Start a new worker thread.
  m_Dispatcher.connect(sigc::mem_fun(*this, &Window::update_tree_view));
  m_WorkerThread = new std::thread([this] { update_values(); });
}

void Window::update_values() {
  while(1) {
    if(stop_work) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (auto &device : devices) {
      device.refresh_sensors();
    }
    m_Dispatcher.emit();
  }
}

void Window::update_tree_view() {
  Gtk::TreeModel::Children rows = m_refTreeModel->children();
  unsigned int device_index = 0;
  for (auto &device_row : rows) {
    // Get Readings
    vector<vector<std::pair<std::string, int>>> readings =
        this->devices[device_index].get_sensor_readings();

    // Get Fan & Temp children of device
    Gtk::TreeModel::Children sensor_types = device_row->children();
    Gtk::TreeModel::Children::iterator iter_sensor_types = sensor_types.begin();

    // Update temp readings
    if (!readings[TEMPERATURE].empty()) {
      Gtk::TreeModel::Children sensors = iter_sensor_types->children();
      Gtk::TreeModel::Children::iterator iter_sensors = sensors.begin();
      for (unsigned int j = 0; j < readings[TEMPERATURE].size();
           j++, iter_sensors++) {
        Gtk::TreeModel::Row values = *iter_sensors;
        values[m_Columns.m_col_value] =
            std::to_string(readings[TEMPERATURE][j].second / 1000);
      }
      iter_sensor_types++;
    }

    // Update fan readings
    if (!readings[FAN].empty()) {
      Gtk::TreeModel::Children sensors = iter_sensor_types->children();
      Gtk::TreeModel::Children::iterator iter_sensors = sensors.begin();
      for (unsigned int j = 0; j < readings[FAN].size(); j++, iter_sensors++) {
        Gtk::TreeModel::Row values = *iter_sensors;
        values[m_Columns.m_col_value] = std::to_string(readings[FAN][j].second);
      }
    }
    device_index++;
  }
}

void Window::on_button_quit() {
  if (m_WorkerThread) {
    // Order the worker thread to stop and wait for it to stop.
    stop_work = true;
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
  }
  hide();
}

void Window::on_treeview_row_activated(const Gtk::TreeModel::Path &path,
                                       Gtk::TreeViewColumn * /* column */) {
  Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    // row[m_Columns.m_col_name] = "poop";
    std::cout << "Row activated: ID=" << row[m_Columns.m_col_name]
      << ", Name=" << row[m_Columns.m_col_value] << std::endl;
  }
}
