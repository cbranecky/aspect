/*
  Copyright (C) 2014 by the authors of the ASPECT code.

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


#ifndef __aspect__heating_model_interface_h
#define __aspect__heating_model_interface_h

#include <aspect/plugins.h>
#include <aspect/material_model/interface.h>

#include <deal.II/base/point.h>
#include <deal.II/base/parameter_handler.h>

namespace aspect
{
  /**
   * A namespace in which we define everything that has to do with defining
   * the heating model.
   *
   * @ingroup HeatingModels
   */
  namespace HeatingModel
  {
    using namespace dealii;

    /**
     * A base class for parameterizations of heating models.
     *
     * @ingroup HeatingModels
     */
    template <int dim>
    class Interface
    {
      public:
        /**
         * Destructor. Made virtual to enforce that derived classes also have
         * virtual destructors.
         */
        virtual ~Interface();

        /**
         * Initialization function. This function is called once at the
         * beginning of the program after parse_parameters is run and after
         * the SimulatorAccess (if applicable) is initialized.
         */
        virtual
        void
        initialize ();

        /**
         * A function that is called at the beginning of each time step. The
         * default implementation of the function does nothing, but derived
         * classes that need more elaborate setups for a given time step may
         * overload the function.
         *
         * The point of this function is to allow complex heating models to do
         * an initialization step once at the beginning of each time step. An
         * example would be a model that take into account the decay of heat
         * generating elements.
         */
        virtual
        void
        update ();

        /**
         * Function to compute the heating terms in @p heating_outputs given the
         * inputs in @p material_model_inputs and the outputs of the material
         * model in @p material_model_outputs. The @p heating_outputs vector is
         * filled with the value of the heating rate at each quadrature point as
         * defined in @p material_model_inputs.
         * The default implementation calls specific_heating_rate to make
         * this implementation backwards compatible.
         */
        virtual
        void
        evaluate (const typename aspect::MaterialModel::Interface<dim>::MaterialModelInputs &material_model_inputs,
                  const typename aspect::MaterialModel::Interface<dim>::MaterialModelOutputs &material_model_outputs,
                  std::vector<double> &heating_outputs) const;

        /**
         * Return the specific heating rate as a function of position.
         */
        virtual
        double
        specific_heating_rate (const double temperature,
                               const double pressure,
                               const std::vector<double> &compositional_fields,
                               const Point<dim> &position) const DEAL_II_DEPRECATED = 0;

        /**
         * Declare the parameters this class takes through input files. The
         * default implementation of this function does not describe any
         * parameters. Consequently, derived classes do not have to overload
         * this function if they do not take any runtime parameters.
         */
        static
        void
        declare_parameters (ParameterHandler &prm);

        /**
         * Read the parameters this class declares from the parameter file.
         * The default implementation of this function does not read any
         * parameters. Consequently, derived classes do not have to overload
         * this function if they do not take any runtime parameters.
         */
        virtual
        void
        parse_parameters (ParameterHandler &prm);
    };




    /**
     * Register a heating model so that it can be selected from the parameter
     * file.
     *
     * @param name A string that identifies the heating model
     * @param description A text description of what this model does and that
     * will be listed in the documentation of the parameter file.
     * @param declare_parameters_function A pointer to a function that can be
     * used to declare the parameters that this heating model wants to read
     * from input files.
     * @param factory_function A pointer to a function that can create an
     * object of this heating model.
     *
     * @ingroup HeatingModels
     */
    template <int dim>
    void
    register_heating_model (const std::string &name,
                            const std::string &description,
                            void (*declare_parameters_function) (ParameterHandler &),
                            Interface<dim> *(*factory_function) ());

    /**
     * A function that given the name of a model returns a pointer to an
     * object that describes it. Ownership of the pointer is transferred to
     * the caller.
     *
     * The model object returned is not yet initialized and has not read its
     * runtime parameters yet.
     *
     * @ingroup HeatingModels
     */
    template <int dim>
    Interface<dim> *
    create_heating_model (ParameterHandler &prm);

    /**
     * Return a list of names of all implemented heating models, separated by
     * '|' so that it can be used in an object of type Patterns::Selection.
     */
    template <int dim>
    std::string
    get_names ();

    /**
     * Declare the runtime parameters of the registered heating models.
     *
     * @ingroup HeatingModels
     */
    template <int dim>
    void
    declare_parameters (ParameterHandler &prm);



    /**
     * Given a class name, a name, and a description for the parameter file
     * for a heating model, register it with the functions that can declare
     * their parameters and create these objects.
     *
     * @ingroup HeatingModels
     */
#define ASPECT_REGISTER_HEATING_MODEL(classname,name,description) \
  template class classname<2>; \
  template class classname<3>; \
  namespace ASPECT_REGISTER_HEATING_MODEL_ ## classname \
  { \
    aspect::internal::Plugins::RegisterHelper<aspect::HeatingModel::Interface<2>,classname<2> > \
    dummy_ ## classname ## _2d (&aspect::HeatingModel::register_heating_model<2>, \
                                name, description); \
    aspect::internal::Plugins::RegisterHelper<aspect::HeatingModel::Interface<3>,classname<3> > \
    dummy_ ## classname ## _3d (&aspect::HeatingModel::register_heating_model<3>, \
                                name, description); \
  }
  }
}


#endif
