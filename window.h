#ifndef WINDOW_H
#define WINDOW_H

#include "graph.h"
#include "tree.h"
#include <gtkmm.h>
#include <thread>
#include <vector>

using std::string;

// Header file is very messy right now...
class Window : public Gtk::Window {
public:
  Window();
  virtual ~Window();

private:
  Glib::Dispatcher m_Dispatcher;
  std::thread *m_WorkerThread;

  vector<Device> devices;
  vector<vector<vector<Device::sensor_reading>>> all_readings;
  vector<string> device_names;
  const std::string file_path = "/sys/class/hwmon/";
  bool stop_work = false;
  Tree *tree;

  void update_values();
  void update_tree();

  // Signal handlers:
  void on_button_quit();

  // Child widgets:
  Gtk::Box m_VBox;

  Gtk::HeaderBar m_headerBar;

  Gtk::StackSwitcher m_stackSwitcher;
  Gtk::Stack m_stack;

  Gtk::Notebook m_Notebook;
  vector<Gtk::Box> m_Notebook_Boxes;
  vector<Graph*> m_Notebook_Temperature_Graphs;
  vector<Graph*> m_Notebook_Fan_Graphs;
  // These need to be below the stack for some reason
  Gtk::ScrolledWindow m_ScrolledWindow_summary;
};

#endif
