__constant sampler_t nearestSampler=CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_NEAREST;

kernel void yuv422torgb(global uchar *src, uint w, uint h, write_only image2d_t dst)
{
//	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint gx = get_global_id(0);
	uint gy = get_global_id(1);

	if((gx < w) && (gy < h))
	{
		float4 p;

		float Y = 1.1643 * (src[gx + gy*w] / 255.0f - 0.0625);
		float Cr = src[gx/2+(gy/2)*(w)+w*h] / 255.0f - 0.5f;
		float Cb = src[gx/2+(gy/2)*(w)+w*h+(w)*(h/2)] / 255.0f - 0.5f;

		p.s0 = Y + 1.5958 * Cb;
		p.s1 = Y - 0.39173*Cr-0.81290*Cb;
		p.s2 = Y + 2.017*Cr;
		p.s3 = 1.0f;

		write_imagef(dst, (int2){ gx, gy }, p);
	}
}

__kernel void yuyvtorgb(global uchar *src, uint width, uint height, __write_only image2d_t dst)
{
	uint gx = get_global_id(0);
	uint x = gx*2;
	uint gy = get_global_id(1);

	if ((x+1 < width) & (gy<height))
	{
//		uint off = gy * width * 2 + x * 2;
		uint off=(gy*width*2)+(x*2);
		float Cb = (255/112)*0.886*(src[off]-128);
		float Y0 = (255/219)*(src[off+1]-16);
		float Cr = (255/112)*0.701*(src[off+2]-128);
		float Y1 = (255/219)*(src[off+3]-16);
		float4 p;

		p.s0 = Y0 + Cr;
		p.s1 = Y0 - (0.114/0.587)*Cb-(0.299/0.587)*Cr;
		p.s2 = Y0 + Cb;
		p.s3 = 1.0f;
		write_imageui(dst, (int2){ x, gy }, convert_uint4(p));

		p.s0 = Y1 + Cr;
		p.s1 = Y1 - (0.114/0.587)*Cb-(0.299/0.587)*Cr;
		p.s2 = Y1 + Cb;
		write_imageui(dst, (int2){ x+1, gy }, convert_uint4(p));
	}
}

__kernel void rgbtoyuyv(__read_only image2d_t src, uint width, uint height, global uchar *dst)
{
	uint gx = get_global_id(0);
	uint x = gx*2;
	uint gy = get_global_id(1);

	if ((x+1 < width) & (gy<height))
	{
		uint off=(gy*width*2)+(x*2);

		uint4 value0=read_imageui(src, nearestSampler, (int2){x, gy});
		uint4 value1=read_imageui(src, nearestSampler, (int2){x+1, gy});

		uint Y0=16+(65.738/256)*value0.x+(129.057/256)*value0.y+(25.064/256)*value0.z;
		uint Cb0=128-(37.945/256)*value0.x-(74.494/256)*value0.y+(112.439/256)*value0.z;
		uint Cr0=128+(112.439/256)*value0.x-(94.154/256)*value0.y-(18.285/256)*value0.z;

		uint Y1=16+(65.738/256)*value1.x+(129.057/256)*value1.y+(25.064/256)*value1.z;
		uint Cb1=128-(37.945/256)*value1.x-(74.494/256)*value1.y+(112.439/256)*value1.z;
		uint Cr1=128+(112.439/256)*value1.x-(94.154/256)*value1.y-(18.285/256)*value1.z;

		dst[off]=(Cb0+Cb1)/2.0;
		dst[off+1]=Y0;
		dst[off+2]=(Cr0+Cr1)/2.0;
		dst[off+3]=Y1;
	}
}
