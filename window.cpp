#include "window.h"
#include <chrono>
#include <experimental/filesystem>
#include <gtkmm.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

namespace fs = std::experimental::filesystem;

Window::Window()
    : m_VBox(Gtk::ORIENTATION_VERTICAL),
      m_VPanedTemperature(Gtk::ORIENTATION_VERTICAL),
      m_VPanedFan(Gtk::ORIENTATION_VERTICAL) {

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
  tree = new Tree(all_readings, device_names);

  // Start a new worker thread.
  m_Dispatcher.connect(sigc::mem_fun(*this, &Window::update_tree));
  m_WorkerThread = new std::thread([this] { update_values(); });

  set_border_width(1);
  set_default_size(500, 600);

  Gtk::Window::set_titlebar(m_headerBar);
  add(m_VBox);

  // Add the TreeView, inside a ScrolledWindow:
  m_ScrolledWindow_summary.add(tree->m_TreeView);

  // Only show the scrollbars when they are necessary:
  m_ScrolledWindow_summary.set_policy(Gtk::POLICY_AUTOMATIC,
                                      Gtk::POLICY_AUTOMATIC);

  // Boxes for temperatures and fans:
  m_VPanedTemperature.pack1(*graph_temperatures);
  m_VPanedTemperature.pack2(*graph_fans);

  // Attempt to make that stack:
  m_stack.add(m_ScrolledWindow_summary, "summary", "Summary");
  m_stack.add(m_VPanedTemperature, "temperatures", "Temperatures");
  // m_stack.add(m_VBoxFan, "fans", "Fans");
  m_stackSwitcher.set_stack(m_stack);
  m_VBox.pack_start(m_stack);

  // HeaderBar:
  m_headerBar.set_show_close_button(true);
  m_headerBar.set_custom_title(m_stackSwitcher);

  show_all_children();
}

Window::~Window() {
  delete tree;
  delete graph_temperatures;
  delete graph_fans;
  // on_button_quit();
  // delete m_WorkerThread;
}

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

void Window::update_tree() {
  tree->update_tree_view(all_readings);
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
