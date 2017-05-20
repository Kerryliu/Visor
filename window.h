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
protected:
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

  Gtk::ScrolledWindow m_ScrolledWindow_summary;

  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_Button_Quit;
};

#endif
