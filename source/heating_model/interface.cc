/*
  Copyright (C) 2011 - 2015 by the authors of the ASPECT code.

  This file is part of ASPECT.

  ASPECT is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  ASPECT is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ASPECT; see the file doc/COPYING.  If not see
  <http://www.gnu.org/licenses/>.
*/


#include <aspect/global.h>
#include <aspect/heating_model/interface.h>

#include <deal.II/base/exceptions.h>
#include <deal.II/base/std_cxx11/tuple.h>

#include <list>


namespace aspect
{
  namespace HeatingModel
  {
    template <int dim>
    Interface<dim>::~Interface ()
    {}


    template <int dim>
    void
    Interface<dim>::initialize ()
    {}



    template <int dim>
    void
    Interface<dim>::update ()
    {}



    template <int dim>
    void
    Interface<dim>::evaluate (const typename MaterialModel::Interface<dim>::MaterialModelInputs &material_model_inputs,
                              const typename MaterialModel::Interface<dim>::MaterialModelOutputs &material_model_outputs,
                              std::vector<double> &heating_outputs) const
    {
      Assert(heating_outputs.size() == material_model_inputs.position.size(),
             ExcMessage ("Heating outputs need to have the same number of entries as the material model inputs."));

      for (unsigned int q=0; q<heating_outputs.size(); ++q)
        heating_outputs[q] = specific_heating_rate(material_model_inputs.temperature[q],
                                                   material_model_inputs.pressure[q],
                                                   material_model_inputs.composition[q],
                                                   material_model_inputs.position[q]);
    }


    template <int dim>
    void
    Interface<dim>::
    declare_parameters (dealii::ParameterHandler &)
    {}


    template <int dim>
    void
    Interface<dim>::parse_parameters (dealii::ParameterHandler &)
    {}


// -------------------------------- Deal with registering heating models and automating
// -------------------------------- their setup and selection at run time

    namespace
    {
      std_cxx11::tuple
      <void *,
      void *,
      internal::Plugins::PluginList<Interface<2> >,
      internal::Plugins::PluginList<Interface<3> > > registered_plugins;
    }


    template <int dim>
    void
    register_heating_model (const std::string &name,
                            const std::string &description,
                            void (*declare_parameters_function) (ParameterHandler &),
                            Interface<dim> *(*factory_function) ())
    {
      std_cxx11::get<dim>(registered_plugins).register_plugin (name,
                                                               description,
                                                               declare_parameters_function,
                                                               factory_function);
    }


    template <int dim>
    Interface<dim> *
    create_heating_model (ParameterHandler &prm)
    {
      std::string model_name;
      prm.enter_subsection ("Heating model");
      {
        model_name = prm.get ("Model name");
      }
      prm.leave_subsection ();

      Interface<dim> *plugin = std_cxx11::get<dim>(registered_plugins).create_plugin (model_name,
                                                                                      "Heating model::Model name");
      return plugin;
    }



    template <int dim>
    std::string
    get_names ()
    {
      return std_cxx11::get<dim>(registered_plugins).get_pattern_of_names ();
    }


    template <int dim>
    void
    declare_parameters (ParameterHandler &prm)
    {
      // declare the actual entry in the parameter file
      prm.enter_subsection ("Heating model");
      {
        const std::string pattern_of_names
          = std_cxx11::get<dim>(registered_plugins).get_pattern_of_names ();
        try
          {
            prm.declare_entry ("Model name", "constant heating",
                               Patterns::Selection (pattern_of_names),
                               "Select one of the following models:\n\n"
                               +
                               std_cxx11::get<dim>(registered_plugins).get_description_string());
          }
        catch (const ParameterHandler::ExcValueDoesNotMatchPattern &)
          {
            // ignore the fact that the default value for this parameter
            // does not match the pattern
          }
      }
      prm.leave_subsection ();

      std_cxx11::get<dim>(registered_plugins).declare_parameters (prm);
    }
  }
}

// explicit instantiations
namespace aspect
{
  namespace internal
  {
    namespace Plugins
    {
      template <>
      std::list<internal::Plugins::PluginList<HeatingModel::Interface<2> >::PluginInfo> *
      internal::Plugins::PluginList<HeatingModel::Interface<2> >::plugins = 0;
      template <>
      std::list<internal::Plugins::PluginList<HeatingModel::Interface<3> >::PluginInfo> *
      internal::Plugins::PluginList<HeatingModel::Interface<3> >::plugins = 0;
    }
  }

  namespace HeatingModel
  {
#define INSTANTIATE(dim) \
  template class Interface<dim>; \
  \
  template \
  void \
  register_heating_model<dim> (const std::string &, \
                               const std::string &, \
                               void ( *) (ParameterHandler &), \
                               Interface<dim> *( *) ()); \
  \
  template  \
  void \
  declare_parameters<dim> (ParameterHandler &); \
  \
  template  \
  std::string \
  get_names<dim> (); \
  \
  template \
  Interface<dim> * \
  create_heating_model<dim> (ParameterHandler &prm);

    ASPECT_INSTANTIATE(INSTANTIATE)
  }
}
