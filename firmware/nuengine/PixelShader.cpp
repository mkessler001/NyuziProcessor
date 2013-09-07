// 
// Copyright 2011-2013 Jeff Bush
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 

#include "PixelShader.h"
#include "Debug.h"

PixelShader::PixelShader(ParameterInterpolator *interp, RenderTarget *target)
	: 	fTarget(target),
		fInterpolator(interp),
		fOneOverWidth(1.0f / target->getColorBuffer()->getWidth()),
		fOneOverHeight(1.0f / target->getColorBuffer()->getHeight()),
		fEnableZBuffer(false)
{
}

void PixelShader::fillMasked(int left, int top, unsigned short mask)
{
	vecf16 outParams[3];
	vecf16 inParams[kMaxParams];
	vecf16 zValues;

	fInterpolator->computeParams(left * fOneOverWidth, top * fOneOverHeight, 
		inParams, zValues);

	if (fEnableZBuffer)
	{
		vecf16 depthBufferValues = (vecf16) fTarget->getZBuffer()->readBlock(left, top);
		int passDepthTest = __builtin_vp_mask_cmpf_lt(zValues, depthBufferValues);
		mask &= passDepthTest;
		fTarget->getZBuffer()->writeBlockMasked(left, top, mask, zValues);
	}

	shadePixels(inParams, outParams, mask);

	// Assume outParams 0, 1, 2, 3 are r, g, b, and a of an output pixel
	// XXX should clamp these...
	veci16 r = __builtin_vp_vftoi(outParams[0] * __builtin_vp_makevectorf(255.0f))
		& __builtin_vp_makevectori(0xff);
	veci16 g = __builtin_vp_vftoi(outParams[1] * __builtin_vp_makevectorf(255.0f))
		& __builtin_vp_makevectori(0xff);
	veci16 b = __builtin_vp_vftoi(outParams[2] * __builtin_vp_makevectorf(255.0f))
		& __builtin_vp_makevectori(0xff);

	veci16 pixelValues = b | (g << __builtin_vp_makevectori(8)) 
		| (r << __builtin_vp_makevectori(16));

	fTarget->getColorBuffer()->writeBlockMasked(left, top, mask, pixelValues);
}
