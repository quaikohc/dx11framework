


// Rectilinear Texture Warped (RTW) images are composed of a conventional base texture enhanced with a rectilinear warping map

// rectilinear warping map is defined by two 1-D warping maps for each major axis (vertical and horizontal)
// the warping maps are composed of a set of super-cells at equal or lower reso-lution than the base texture

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}