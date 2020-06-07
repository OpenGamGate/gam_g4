// --------------------------------------------------
//   Copyright (C): OpenGATE Collaboration
//   This software is distributed under the terms
//   of the GNU Lesser General  Public Licence (LGPL)
//   See LICENSE.md for further details
// --------------------------------------------------

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "G4UImanager.hh"
#include "G4UIcommandTree.hh"

void init_G4UImanager(py::module &m) {

    py::class_<G4UImanager>(m, "G4UImanager")

      .def("GetUIpointer", &G4UImanager::GetUIpointer, py::return_value_policy::reference)
        //.staticmethod("GetUIpointer")

      .def("GetCurrentValues", &G4UImanager::GetCurrentValues)
      .def("ExecuteMacroFile", &G4UImanager::ExecuteMacroFile)

        // .def("ApplyCommand",     f1_ApplyCommand)
        // .def("ApplyCommand",     f2_ApplyCommand)

      .def("ApplyCommand", py::overload_cast<const char *>(&G4UImanager::ApplyCommand))
      .def("ApplyCommand", py::overload_cast<const G4String &>(&G4UImanager::ApplyCommand))

        //.def("CreateHTML",       &G4UImanager::CreateHTML, f_CreateHTML())
      .def("SetMacroSearchPath", &G4UImanager::SetMacroSearchPath)
      .def("GetMacroSearchPath", &G4UImanager::GetMacroSearchPath, py::return_value_policy::copy)
      .def("SetPauseAtBeginOfEvent", &G4UImanager::SetPauseAtBeginOfEvent)
      .def("GetPauseAtBeginOfEvent", &G4UImanager::GetPauseAtBeginOfEvent)
      .def("SetPauseAtEndOfEvent", &G4UImanager::SetPauseAtEndOfEvent)
      .def("GetPauseAtEndOfEvent", &G4UImanager::GetPauseAtEndOfEvent)
      .def("SetVerboseLevel", &G4UImanager::SetVerboseLevel)
      .def("GetVerboseLevel", &G4UImanager::GetVerboseLevel)
      .def("GetTree", &G4UImanager::GetTree, py::return_value_policy::reference);

    // ---
    //def("ApplyUICommand",    ApplyUICommand_1);
    //def("ApplyUICommand",    ApplyUICommand_2);

}
