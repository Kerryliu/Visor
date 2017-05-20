#ifndef WINDOW_H
#define WINDOW_H

#include "tree.h"
#include "graph.h"
#include <gtkmm.h>

using std::string;

// Header file is very messy right now...
class Window : public Gtk::Window {
public:
  Window();
  virtual ~Window();
private:
  Tree* tree = new Tree();
  Graph* graph_temperatures = new Graph();
  Graph* graph_fans = new Graph();
  // Signal handlers:
  void on_button_quit();


  // Child widgets:
  Gtk::Box m_VBox;

  Gtk::HeaderBar m_headerBar;

  Gtk::StackSwitcher m_stackSwitcher;
  Gtk::Stack m_stack;

  // These need to be below the stack for some reason
  Gtk::Paned m_VPanedTemperature;
  Gtk::Paned m_VPanedFan;
  Gtk::ScrolledWindow m_ScrolledWindow_summary;
};

#endif
