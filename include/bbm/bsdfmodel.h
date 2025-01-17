#ifndef _BBM_BSDFMODEL_H_
#define _BBM_BSDFMODEL_H_

#include "concepts/macro.h"
#include "concepts/bsdfmodel.h"

#include "util/reflection.h"
#include "util/toString.h"

#include "core/spherical.h"
#include "core/transform.h"
#include "core/precompute.h"
#include "core/vec_transform.h"
#include "core/shading_frame.h"

#include "bbm/unit.h"
#include "bbm/config.h"
#include "bbm/fresnel.h"
#include "bbm/constructor.h"
#include "bbm/bsdfsample.h"
#include "bbm/bsdf_attribute.h"
#include "bbm/bsdf_symmetry.h"


/************************************************************************/
/*! \file bsdfmodel.h

  \brief All includes and helpers needed for declaring new bsdfmodels

  Convention regarding 'in' and 'out directions. BSDF models follow the in-out
    convention of Eric Veach, namely 'in' is the sampled direction, and
    transport flows from -in to out.  The physical meaning of 'in' and 'out'
    switches between the regular bsdf (Radiance) and the adjoint bsdf
    (Importance).

  Thus for unit_t:Radiance: in == light and out = view.
    
  Printing of bsdfmodels starts with the name followed by:
  + if the bsdfmodel has a toString() method, then use this.
  + else if the bsdfmodel support reflection, then print name and attributes
  + else print empty '()'
    
*************************************************************************/

namespace bbm{
  namespace detail {

    //! @{ \name Predefine common default argument types; this will speed up compilation significantly
    using DefBsdfAll = ArgDef(bbm::bsdf_flag::All);
    using DefUnitRad = ArgDef(bbm::unit_t::Radiance);
    using DefTrue = ArgDef(true);
    //! @}
    
  } // end detail namespace
} // end bbm namespace

/************************************************************************/
/*! \brief Helper Macro to forward bbm::args arguments to eval, sample, pdf,
    and reflectance
*************************************************************************/
#define BBM_BSDF_FORWARD                                                 \
  BBM_FORWARD_CPP_ARGS_CONST(eval, bbm::arg<const Vec3d&, "in">,         \
                             bbm::arg<const Vec3d&, "out">,              \
                             bbm::arg<BsdfFlag, "component", bbm::detail::DefBsdfAll>, \
                             bbm::arg<bbm::unit_t, "unit", bbm::detail::DefUnitRad>, \
                             bbm::arg<Mask, "mask", bbm::detail::DefTrue>); \
  BBM_FORWARD_CPP_ARGS_CONST(sample, bbm::arg<const Vec3d&, "out">, \
                             bbm::arg<const Vec2d&, "xi">,               \
                             bbm::arg<BsdfFlag, "component", bbm::detail::DefBsdfAll>, \
                             bbm::arg<bbm::unit_t, "unit", bbm::detail::DefUnitRad>, \
                             bbm::arg<Mask, "mask", bbm::detail::DefTrue>); \
  BBM_FORWARD_CPP_ARGS_CONST(pdf, bbm::arg<const Vec3d&, "in">,           \
                             bbm::arg<const Vec3d&, "out">,               \
                             bbm::arg<BsdfFlag, "component", bbm::detail::DefBsdfAll>, \
                             bbm::arg<bbm::unit_t, "unit", bbm::detail::DefUnitRad>, \
                             bbm::arg<Mask, "mask", bbm::detail::DefTrue>); \
  BBM_FORWARD_CPP_ARGS_CONST(reflectance, bbm::arg<const Vec3d&, "out">,  \
                             bbm::arg<BsdfFlag, "component", bbm::detail::DefBsdfAll>, \
                             bbm::arg<bbm::unit_t, "unit", bbm::detail::DefUnitRad>, \
                             bbm::arg<Mask, "mask", bbm::detail::DefTrue>); \

                             
namespace bbm {

  using namespace bbm::reflection;
  
  template<typename BSDFMODEL> requires bbm::concepts::bsdfmodel<BSDFMODEL>
    std::ostream& operator<<(std::ostream& s, const BSDFMODEL& model)
  {
    s << std::decay_t<BSDFMODEL>::name;
    
    // toString method has preference
    if constexpr (has_toString<BSDFMODEL>) s << model.toString();

    // otherwise check if has reflection, print attributes
    else if constexpr (bbm::concepts::reflection::supported<BSDFMODEL>) s << bbm::reflection::attributes(model);

    // otherwise print empty '()'
    else s << "()";
    
    // Done.
    return s;
  }

} // end std namespace

#endif /* _BBM_BSDFMODEL_H_ */
