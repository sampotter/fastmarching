// Extra definitions that aren't generated using the Python
// script---this file just exists to make it easier to read and write
// them (i.e. without having to edit a Python string embedded in a
// larger script)

py::enum_<state>(m, "State")
  .value("Valid", state::valid)
  .value("Trial", state::trial)
  .value("Far", state::far);

// We could probably clean this up a little by exposing the
// abstract_node base class here.

py::class_<node>(m, "Node")
  .def_property("value", &node::get_value, &node::set_value)
  .def_property("state", &node::get_state, &node::set_state)
  .def_property_readonly("i", &node::get_i)
  .def_property_readonly("j", &node::get_j);

py::class_<node_3d>(m, "Node3d")
  .def_property("value", &node::get_value, &node::set_value)
  .def_property("state", &node::get_state, &node::set_state)
  .def_property_readonly("i", &node_3d::get_i)
  .def_property_readonly("j", &node_3d::get_j)
  .def_property_readonly("k", &node_3d::get_k)
#if TRACK_PARENTS
  .def("get_parent_index", &node_3d::get_parent_index, "i"_a)
#endif
  ;
