#include "surface.h"


void ag_surface_filter_to(struct ag_surface* dst, struct ag_surface* src, enum ag_filter scaler)
{
	switch(scaler)
	{
	case AG_FILTER_UP2_NN:
		for(int y = 0; y < src->size.h; ++y)
			for(int x = 0; x < src->size.w; ++x)
			{
				uint32_t c = ((uint32_t*)src->data)[(x+0)+(y+0)*src->size.w];
				((uint32_t*)dst->data)[(x*2+0)+(y*2+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*2+1)+(y*2+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*2+0)+(y*2+1)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*2+1)+(y*2+1)*dst->size.w] = c;
			}
		break;
	case AG_FILTER_UP3_NN: 
		for(int y = 0; y < src->size.h; ++y)
			for(int x = 0; x < src->size.w; ++x)
			{
				uint32_t c = ((uint32_t*)src->data)[(x+0)+(y+0)*src->size.w];
				((uint32_t*)dst->data)[(x*3+0)+(y*3+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*3+1)+(y*3+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*3+2)+(y*3+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*3+0)+(y*3+1)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*3+1)+(y*3+1)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*3+2)+(y*3+1)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*3+0)+(y*3+2)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*3+1)+(y*3+2)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*3+2)+(y*3+2)*dst->size.w] = c;
			}
		break;
	case AG_FILTER_UP4_NN:
		for(int y = 0; y < src->size.h; ++y)
			for(int x = 0; x < src->size.w; ++x)
			{
				uint32_t c = ((uint32_t*)src->data)[(x+0)+(y+0)*src->size.w];
				((uint32_t*)dst->data)[(x*4+0)+(y*4+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+1)+(y*4+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+2)+(y*4+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+3)+(y*4+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+0)+(y*4+1)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+1)+(y*4+1)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+2)+(y*4+1)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+3)+(y*4+1)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+0)+(y*4+2)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+1)+(y*4+2)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+2)+(y*4+2)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+3)+(y*4+2)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+0)+(y*4+3)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+1)+(y*4+3)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+2)+(y*4+3)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*4+3)+(y*4+3)*dst->size.w] = c;
			}
		break;
	case AG_FILTER_UP2_LIN:
		for(int y = 0; y < src->size.h; ++y)
			for(int x = 0; x < src->size.w; ++x)
			{
				uint32_t l, r, t, b, c;
				t = ((uint32_t*)src->data)[(x+0)+(y-(y==0?0:1))*src->size.w];
				l = ((uint32_t*)src->data)[(x-(x==0?0:1))+(y+0)*src->size.w];
				c = ((uint32_t*)src->data)[(x+0)+(y+0)*src->size.w];
				r = ((uint32_t*)src->data)[(x+(x==src->size.w-1?0:1))+(y+0)*src->size.w];
				b = ((uint32_t*)src->data)[(x+0)+(y+(y==src->size.h-1?0:1))*src->size.w];
				c = (c&0xfefefefe) >> 1;
				l = (l&0xfcfcfcfc) >> 2;
				r = (r&0xfcfcfcfc) >> 2;
				t = (t&0xfcfcfcfc) >> 2;
				b = (b&0xfcfcfcfc) >> 2;
				((uint32_t*)dst->data)[(x*2+0)+(y*2+0)*dst->size.w] = c+l+t;
				((uint32_t*)dst->data)[(x*2+1)+(y*2+0)*dst->size.w] = c+r+t;
				((uint32_t*)dst->data)[(x*2+0)+(y*2+1)*dst->size.w] = c+l+b;
				((uint32_t*)dst->data)[(x*2+1)+(y*2+1)*dst->size.w] = c+r+b;
			}
		break;
	case AG_FILTER_UP2_RGB:
		for(int y = 0; y < src->size.h; ++y)
			for(int x = 0; x < src->size.w; ++x)
			{
				/*
				struct ag_color c = src->data[(x+0)+(y+0)*src->size.w];
				dst->data[(x*2+0)+(y*2+0)*dst->size.w] =	c;
				dst->data[(x*2+1)+(y*2+0)*dst->size.w] =	(struct ag_color){.r=c.r, .b=0, .g=0, .a=c.a};
				dst->data[(x*2+0)+(y*2+1)*dst->size.w] =	(struct ag_color){.r=0, .b=c.b, .g=0, .a=c.a};
				dst->data[(x*2+1)+(y*2+1)*dst->size.w] =	(struct ag_color){.r=0, .b=0, .g=c.g, .a=c.a};
				*/
				uint32_t c = ((uint32_t*)src->data)[(x+0)+(y+0)*src->size.w];
				((uint32_t*)dst->data)[(x*2+0)+(y*2+0)*dst->size.w] = c;
				((uint32_t*)dst->data)[(x*2+1)+(y*2+0)*dst->size.w] = c&0xffff0000L;
				((uint32_t*)dst->data)[(x*2+0)+(y*2+1)*dst->size.w] = c&0xff00ff00L;
				((uint32_t*)dst->data)[(x*2+1)+(y*2+1)*dst->size.w] = c&0xff0000ffL;
				
			}
		break;	
	case AG_FILTER_UP2_SCALE2X:
		for(int y = 0; y < src->size.h; ++y)
			for(int x = 0; x < src->size.w; ++x)
			{
				uint32_t b, d, e, f, h, e0, e1, e2, e3;
				b = ((uint32_t*)src->data)[(x+0)+(y-(y==0?0:1))*src->size.w];
				d = ((uint32_t*)src->data)[(x-(x==0?0:1))+(y+0)*src->size.w];
				e = ((uint32_t*)src->data)[(x+0)+(y+0)*src->size.w];
				f = ((uint32_t*)src->data)[(x+(x==src->size.w-1?0:1))+(y+0)*src->size.w];
				h = ((uint32_t*)src->data)[(x+0)+(y+(y==src->size.h-1?0:1))*src->size.w];

				if (b != h && d != f)
				{
					e0 = d == b ? d : e;
					e1 = b == f ? f : e;
					e2 = d == h ? d : e;
					e3 = h == f ? f : e;
				}
				else
				{
					e0 = e1 = e2 = e3 = e;
				}
				((uint32_t*)dst->data)[(x*2+0)+(y*2+0)*dst->size.w] = e0;
				((uint32_t*)dst->data)[(x*2+1)+(y*2+0)*dst->size.w] = e1;
				((uint32_t*)dst->data)[(x*2+0)+(y*2+1)*dst->size.w] = e2;
				((uint32_t*)dst->data)[(x*2+1)+(y*2+1)*dst->size.w] = e3;
			}
		break;
	case AG_FILTER_DOWN2_TOPLEFT:
		for(int y = 0; y < dst->size.h; ++y)
			for(int x = 0; x < dst->size.w; ++x)
				((uint32_t*)dst->data)[x+y*dst->size.w] = ((uint32_t*)src->data)[(x*2)+(y*2)*src->size.w];
		break;
	case AG_FILTER_DOWN2_AVG:
		for(int y = 0; y < dst->size.h; ++y)
			for(int x = 0; x < dst->size.w; ++x)
				((uint32_t*)dst->data)[x+y*dst->size.w] =
				  ((((uint32_t*)src->data)[(x*2+0)+(y*2+0)*src->size.w]&0xfcfcfcfc) >> 2) +
				  ((((uint32_t*)src->data)[(x*2+1)+(y*2+0)*src->size.w]&0xfcfcfcfc) >> 2) +
				  ((((uint32_t*)src->data)[(x*2+0)+(y*2+1)*src->size.w]&0xfcfcfcfc) >> 2) +
				  ((((uint32_t*)src->data)[(x*2+1)+(y*2+1)*src->size.w]&0xfcfcfcfc) >> 2);
		break;
	case AG_FILTER_HUESHIFT:
		for(int y = 0; y < src->size.h; ++y)
			for(int x = 0; x < src->size.w; ++x)
				dst->data[x+y*dst->size.w] = ag_hue_shift(src->data[x+y*src->size.w], 30.f);
	}
}

double ag_filter_get_scale(enum ag_filter filter)
{
	switch(filter)
	{
	case AG_FILTER_UP2_NN:
	case AG_FILTER_UP2_LIN:
	case AG_FILTER_UP2_RGB:
	case AG_FILTER_UP2_SCALE2X:
		return 2.0;
	case AG_FILTER_UP3_NN:
		return 3.0;
	case AG_FILTER_UP4_NN:
		return 4.0;
	case AG_FILTER_DOWN2_TOPLEFT:
	case AG_FILTER_DOWN2_AVG:
		return 0.5;
	case AG_FILTER_HUESHIFT:
		return 1.0;
	}
	return 1.0;
}
