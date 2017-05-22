#include "graph.h"
#include <gtkmm/drawingarea.h>

Graph::Graph(const Device &device, int type) : type(type), device(device) {}

Graph::~Graph() {}

// https://developer.gnome.org/gtkmm-tutorial/stable/chapter-drawingarea.html.en
bool Graph::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();

  draw_title(cr);
  return true;
}

void Graph::draw_title(const Cairo::RefPtr<Cairo::Context> &cr) {
  const int left_offset = 10;
  const int top_offset = 10;
  const double fifty_shades_of_grey = 0.4;
  cr->set_source_rgb(fifty_shades_of_grey, fifty_shades_of_grey,
                     fifty_shades_of_grey);

  Pango::FontDescription font;
  font.set_weight(Pango::WEIGHT_BOLD);
  auto layout = create_pango_layout(Device::sensor_types[type]);

  layout->set_font_description(font);

  int text_width;
  int text_height;

  // get the text dimensions (it updates the variables -- by reference)
  layout->get_pixel_size(text_width, text_height);

  cr->move_to(left_offset, top_offset);

  layout->show_in_cairo_context(cr);
}
