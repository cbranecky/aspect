//-------------------------------------------------------------
//    $Id$
//
//    Copyright (C) 2011 by the authors of the ASPECT code
//
//-------------------------------------------------------------

#include <aspect/boundary_temperature/spherical_constant.h>
#include <aspect/geometry_model/spherical_shell.h>

#include <utility>
#include <limits>


namespace aspect
{
  namespace BoundaryTemperature
  {
// ------------------------------ SphericalConstant -------------------

    template <int dim>
    double
    SphericalConstant<dim>::
    temperature (const GeometryModel::Interface<dim> &geometry_model,
                 const unsigned int                   boundary_indicator,
                 const Point<dim>                    &location) const
    {
      // verify that the geometry is in fact a spherical shell since only
      // for this geometry do we know for sure what boundary indicators it
      // uses and what they mean
      Assert (dynamic_cast<const GeometryModel::SphericalShell<dim>*>(&geometry_model)
              != 0,
              ExcMessage ("This boundary model is only implemented if the geometry is "
                          "in fact a spherical shell."));

      switch (boundary_indicator)
        {
          case 0:
            return inner_temperature;
          case 1:
            return outer_temperature;
          default:
            Assert (false, ExcMessage ("Unknown boundary indicator."));
            return std::numeric_limits<double>::quiet_NaN();
        }
    }


    template <int dim>
    double
    SphericalConstant<dim>::
    minimal_temperature () const
    {
      return std::min (inner_temperature, outer_temperature);
    }



    template <int dim>
    double
    SphericalConstant<dim>::
    maximal_temperature () const
    {
      return std::max (inner_temperature, outer_temperature);
    }



    template <int dim>
    void
    SphericalConstant<dim>::declare_parameters (ParameterHandler &prm)
    {
      prm.enter_subsection("Boundary temperature model");
      {
        prm.enter_subsection("Spherical constant");
        {
          prm.declare_entry ("Outer temperature", "0",
                             Patterns::Double (),
                             "Temperature at the outer boundary (lithosphere water/air). Units: Kelvin.");
          prm.declare_entry ("Inner temperature", "6000",
                             Patterns::Double (),
                             "Temperature at the inner boundary (core mantle boundary). Units: Kelvin.");
        }
        prm.leave_subsection ();
      }
      prm.leave_subsection ();
    }


    template <int dim>
    void
    SphericalConstant<dim>::parse_parameters (ParameterHandler &prm)
    {
      prm.enter_subsection("Boundary temperature model");
      {
        prm.enter_subsection("Spherical constant");
        {
          inner_temperature = prm.get_double ("Inner temperature");
          outer_temperature = prm.get_double ("Outer temperature");
        }
        prm.leave_subsection ();
      }
      prm.leave_subsection ();
    }
  }
}

// explicit instantiations
namespace aspect
{
  namespace BoundaryTemperature
  {
    template class SphericalConstant<deal_II_dimension>;
    ASPECT_REGISTER_BOUNDARY_TEMPERATURE_MODEL("spherical constant", SphericalConstant);
  }
}