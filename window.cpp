#include "window.h"

namespace fs = std::experimental::filesystem;

Window::Window() : m_VBox(Gtk::ORIENTATION_VERTICAL), m_Button_Quit("Quit") {
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

  for (auto &p : fs::directory_iterator(path)) {
    devices.push_back(Device(p.path().string()));
  }

  make_tree_view();

  show_all_children();

}

Window::~Window() {}

void Window::make_tree_view() {
  // Create the Tree model:
  m_refTreeModel.clear();
  m_refTreeModel = Gtk::TreeStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);

  Gtk::TreeModel::Row row;
  Gtk::TreeModel::Row child_row;
  Gtk::TreeModel::Row baby_child_row;

  for (unsigned int i = 0; i < devices.size(); i++) {
    // Get Readings
    std::vector<std::vector<std::pair<std::string, int>>> readings =
        devices[i].get_sensor_readings();
    std::vector<std::pair<std::string, int>> temp_readings =
        readings[TEMPERATURE];
    std::vector<std::pair<std::string, int>> fan_readings = readings[FAN];

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_name] = devices[i].name;
    row[m_Columns.m_col_value] = "";

    // Temperature
    if (!temp_readings.empty()) {
      child_row = *(m_refTreeModel->append(row.children()));
      child_row[m_Columns.m_col_name] = "Temperature: ";
      child_row[m_Columns.m_col_value] = "";
      for (unsigned int j = 0; j < temp_readings.size(); j++) {
        baby_child_row = *(m_refTreeModel->append(child_row.children()));
        baby_child_row[m_Columns.m_col_name] = temp_readings[j].first + ": ";
        baby_child_row[m_Columns.m_col_value] =
            std::to_string(temp_readings[j].second / 1000);
      }
    }

    // Fan
    if (!fan_readings.empty()) {
      child_row = *(m_refTreeModel->append(row.children()));
      child_row[m_Columns.m_col_name] = "Fan: ";
      child_row[m_Columns.m_col_value] = "";
      for (unsigned int j = 0; j < fan_readings.size(); j++) {
        baby_child_row = *(m_refTreeModel->append(child_row.children()));
        baby_child_row[m_Columns.m_col_name] = fan_readings[j].first + ": ";
        baby_child_row[m_Columns.m_col_value] =
            std::to_string(fan_readings[j].second);
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
  m_TreeView.set_fixed_height_mode(true);
  m_TreeView.set_enable_tree_lines(true);

  sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &Window::update_tree_view), test);
  sigc::connection conn = Glib::signal_timeout().connect(my_slot, 1000);
}

bool Window::update_tree_view(int x) {
  m_TreeView.freeze_notify();
  Gtk::TreeModel::Children devices = m_refTreeModel->children();
  unsigned int device_index = 0;
  for(auto device : devices) {
    // Get Readings
    std::vector<std::vector<std::pair<std::string, int>>> readings =
        this->devices[device_index].get_sensor_readings();
    std::vector<std::pair<std::string, int>> temp_readings =
        readings[TEMPERATURE];
    std::vector<std::pair<std::string, int>> fan_readings = readings[FAN];

    //Get Fan & Temp children of device
    Gtk::TreeModel::Children sensor_types = device->children();
    Gtk::TreeModel::Children::iterator iter_sensor_types = sensor_types.begin();

    // Update temp readings
    if (!temp_readings.empty()) {
      Gtk::TreeModel::Children sensors = iter_sensor_types->children();
      Gtk::TreeModel::Children::iterator iter_sensors = sensors.begin();
      for (unsigned int j = 0; j < temp_readings.size(); j++, iter_sensors++) {
        Gtk::TreeModel::Row values = *iter_sensors;
        values[m_Columns.m_col_value] =
            std::to_string(temp_readings[j].second / 1000);
      }
      iter_sensor_types++;
    }
    if (!fan_readings.empty()) {
      Gtk::TreeModel::Children sensors = iter_sensor_types->children();
      Gtk::TreeModel::Children::iterator iter_sensors = sensors.begin();
      for (unsigned int j = 0; j < fan_readings.size(); j++, iter_sensors++) {
        Gtk::TreeModel::Row values = *iter_sensors;
        values[m_Columns.m_col_value] =
            std::to_string(fan_readings[j].second);
      }
    }
    device_index++;
  }
  std::cout << test++ << std::endl;
  m_TreeView.thaw_notify();
  return true;
}

void Window::on_button_quit() { hide(); }

void Window::on_treeview_row_activated(const Gtk::TreeModel::Path &path,
                                       Gtk::TreeViewColumn * /* column */) {
  Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
  if (iter) {
    //Gtk::TreeModel::Row row = *iter;
    //row[m_Columns.m_col_name] = "poop";
    //std::cout << "Row activated: ID=" << row[m_Columns.m_col_name]
              //<< ", Name=" << row[m_Columns.m_col_value] << std::endl;
  }
}
