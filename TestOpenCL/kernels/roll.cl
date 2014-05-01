
__constant sampler_t imageSampler=CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_NEAREST;

__kernel void rollHorizontal(__read_only image2d_t input, __write_only image2d_t output, int pos)
{
	int2 coord=(int2)(get_global_id(0), get_global_id(1));
	int2 imagePt=coord;
	uint4 value;

	imagePt.x+=pos;
	if(imagePt.x > get_global_size(0))
		imagePt.x-=get_global_size(0);

	value=read_imageui(input, imageSampler, imagePt);

	//swap pixel order (QImage in BGRA)
	uint temp=value.x;
	value.x=value.z;
	value.z=temp;
	write_imageui(output, coord, value);
}