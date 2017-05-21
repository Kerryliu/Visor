#ifndef GRAPH_H
#define GRAPH_H

#include <gtkmm/drawingarea.h>

class Graph : public Gtk::DrawingArea {
public:
  Graph();
  virtual ~Graph();
  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
};

#endif
