#include "device.h"
#include "graph.h"
#include "tree.h"
#include "window.h"
#include <chrono>
#include <experimental/filesystem>
#include <gtkmm.h>
#include <iostream>
#include <thread>
#include <vector>

namespace fs = std::experimental::filesystem;

Window::Window() : m_VBox(Gtk::ORIENTATION_VERTICAL) {

  // Find all devices:
  for (auto &p : fs::directory_iterator(file_path)) {
    devices.push_back(Device(p.path().string()));
  }
  // Get initial values:
  for (unsigned int device_index = 0; device_index < devices.size();
       device_index++) {
    devices[device_index].refresh_sensors();
    all_readings.push_back(devices[device_index].get_sensor_readings());
    device_names.push_back(devices[device_index].name);
  }
  tree = std::make_unique<Tree>(all_readings, device_names);

  // Start a new worker thread.
  m_Dispatcher.connect(sigc::mem_fun(*this, &Window::update_tree));

  set_border_width(1);
  set_default_size(500, 600);

  Gtk::Window::set_titlebar(m_headerBar);
  m_VBox.set_size_request(500, 500);
  add(m_VBox);

  // Add the TreeView, inside a ScrolledWindow:
  m_ScrolledWindow_summary.add(tree->m_TreeView);

  // Only show the scrollbars when they are necessary:
  m_ScrolledWindow_summary.set_policy(Gtk::POLICY_AUTOMATIC,
                                      Gtk::POLICY_AUTOMATIC);
  // Setup that notebook
  m_Notebook_Graphs.resize(devices.size());
  for (unsigned int page = 0; page < devices.size(); page++) {
    m_Notebook_Boxes.push_back(Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    vector<vector<Device::sensor_reading>> device_readings =
        devices[page].get_sensor_readings();
    for (unsigned int sensor_type = 1; sensor_type < device_readings.size();
         sensor_type += 2) { // Skip voltage and pwm
      if (!device_readings[sensor_type].empty()) {
        m_Notebook_Graphs[page].push_back(
            std::make_unique<Graph>(device_readings[sensor_type], sensor_type));
      }
    }
    for (auto &graph : m_Notebook_Graphs[page]) {
      m_Notebook_Boxes[page].pack_start(*graph, true, true);
    }
    m_Notebook.append_page(m_Notebook_Boxes[page], devices[page].name);
  }

  // Attempt to make that stack:
  m_stack.add(m_ScrolledWindow_summary, "summary", "Summary");
  m_stack.add(m_Notebook, "graphs", "Graphs");
  m_stackSwitcher.set_stack(m_stack);
  m_VBox.pack_start(m_stack);

  // HeaderBar:
  m_headerBar.set_show_close_button(true);
  m_headerBar.set_custom_title(m_stackSwitcher);

  show_all_children();
}

Window::~Window() { on_button_quit(); }

void Window::update_values() {
  while (1) {
    if (stop_work) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (unsigned int device_index = 0; device_index < devices.size();
         device_index++) {
      devices[device_index].refresh_sensors();
      all_readings[device_index] = devices[device_index].get_sensor_readings();
    }
    m_Dispatcher.emit();
  }
}

void Window::update_tree() { tree->update_tree_view(all_readings); }

void Window::update_graph() { std::cout << "hmmm" << std::endl; }

void Window::on_button_quit() {
  if (m_WorkerThread) {
    // Order the worker thread to stop and wait for it to stop.
    stop_work = true;
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
  }
  hide();
}
