#include "device.h"
#include "graph.h"
#include "legend.h"
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
  m_Dispatcher.connect(sigc::mem_fun(*this, &Window::update_all));

  set_border_width(1);
  set_default_size(500, 600);
  Gtk::Window::set_titlebar(m_headerBar);
  add(m_VBox);
  // Add the TreeView, inside a ScrolledWindow:
  m_ScrolledWindow.add(tree->m_TreeView);

  // Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  // Setup that notebook
  m_notebook_graphs.resize(devices.size());
  m_notebook_legends.resize(devices.size());
  for (unsigned int page = 0; page < devices.size(); page++) {
    m_Notebook_Boxes.push_back(Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    vector<vector<Device::sensor_reading>> device_readings =
        devices[page].get_sensor_readings();
    for (unsigned int sensor_type = 0; sensor_type < device_readings.size();
         sensor_type++) { // Skip voltage and pwm
      if (!device_readings[sensor_type].empty()) {
        m_notebook_graphs[page].push_back(std::make_unique<Graph>(
            device_readings[sensor_type], page, sensor_type));
        m_notebook_legends[page].push_back(std::make_unique<Legend>(
            device_readings[sensor_type], page, sensor_type, 500));
      }
    }
    for (unsigned int i = 0; i < m_notebook_graphs[page].size(); i++) {
      m_Notebook_Boxes[page].pack_start(*m_notebook_graphs[page][i]);
      m_Notebook_Boxes[page].pack_start(m_notebook_legends[page][i]->m_legend,
                                        false, false);
    }
    m_Notebook.append_page(m_Notebook_Boxes[page], devices[page].name);
  }

  // Make that stack:
  m_stack.add(m_ScrolledWindow, "summary", "Summary");
  m_stack.add(m_Notebook, "graphs", "Graphs");
  m_stackSwitcher.set_stack(m_stack);
  m_VBox.pack_start(m_stack);

  // HeaderBar:
  m_headerBar.set_show_close_button(true);
  m_headerBar.set_custom_title(m_stackSwitcher);
  show_all_children();
}

Window::~Window() { on_button_quit(); }

void Window::update_vals() {
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

void Window::update_all() {
  tree->update_tree_view(all_readings);
  for (auto &page : m_notebook_graphs) {
    for (auto &graph : page) {
      unsigned int type = graph->get_type();
      unsigned int device_index = graph->get_device_index();
      graph->update_vals(all_readings[device_index][type]);
    }
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
