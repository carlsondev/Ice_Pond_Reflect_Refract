
# Paper Tasks

* The EM (environment mapping) texture is updated by rendering the entire scene six times with a field of view set to 90°,
  rotating the camera to point in all six directions from the centre of the ice object.


$$ n_r = n_f (1-a) - n_b (a) $$
* $n_r$ is the resuling normal
* $n_f$ is the normal of the front face in world space
* $n_b$ is the normal of the back face in world space, sampled from the texture
* $a$ is a scalar value in the range of [0, 1] indicating how much of the back face’s normal that should be applied. A
  value of 0.33 is used for $a$ in the screenshots in this report. 

$$ r_{\rm refr} = eta \cdot e - (eta(n_r \cdot e) + \sqrt{1 - eta^2(1 - (n_r \cdot e)^2)}) \cdot n_r $$
* $r_{\rm refr}$ is the refracted eye vector
* $eta$ is the refraction index of the media we are travelling
  from divided by the media we are travelling to. In this
  case air/ice = $1.0003/1.3091 \approx 0.7641$

$r_{\rm refr}$ is then used to sample the environment map


So instead of following the incident ray and refracting it twice, a
new normal is simply created by combining the back and front
face’s normal vectors. This is not a physically correct method but
since ice is a highly distorted material this is a reasonable trade
off for increased performance. Also, highly concave objects will
not refract correctly since only one front face and one back face
are accounted for.
For the purpose of reflection calculations, only the normal vector
of the front face needs to be accounted for. So e is simply
reflected using equation 3.

$$ r_{\rm refl} = e - 2(n_f \cdot e) \cdot n_f $$

where $r_{\rm refl}$ is the reflected eye vector.


Reflected and refracted light is sampled by using the reflection
and refraction vectors as indexes into the environmental map. The
resulting two color samples are then mixed using an
approximation of the Fresnel equation similar to the one proposed
in [Jensen and Golias 2001]. The Fresnel equation determines the
ratio between the reflected and refracted ray’s color contribution
using a rather heavy equation. Our approximation is created with
the intention of being fast and yet to produce a convincing visual
result rather than being physically correct (equation 4).

$$ f = (1 - |e \cdot n |)^3 $$

where $f$ is the Fresnel factor, $e$ is the eye vector and $n$ is the normal vector.

Equation 4 is calculated in the ice object’s vertex shader and f is
interpolated and sent to the fragment shader where it is used for
mixing the reflection and refraction colors sampled from the
environment map (equation 6). The result is shown in Figure 4. 