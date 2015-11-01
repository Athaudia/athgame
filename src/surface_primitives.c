#include "surface.h"
#include <math.h>
#include <stdlib.h>

void ag_surface32__fill_rect(struct ag_surface32* surface, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color32 color)
{
	for(int y = 0; y < size.h; ++y)
		for(int x = 0; x < size.w; ++x)
			surface->data[(x+pos.x)+(y+pos.y)*surface->size.w] = color;
}

void ag_surface32__draw_rect(struct ag_surface32* surface, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color32 color)
{
	size.w--;
	size.h--;
	ag_surface32__draw_line(surface, pos, ag_vec2i(pos.x,pos.y+size.h), color);
	ag_surface32__draw_line(surface, pos, ag_vec2i(pos.x+size.w,pos.y), color);
	ag_surface32__draw_line(surface, ag_vec2i(pos.x,pos.y+size.h), ag_vec2i(pos.x+size.w,pos.y+size.h), color);
	ag_surface32__draw_line(surface, ag_vec2i(pos.x+size.w,pos.y), ag_vec2i(pos.x+size.w,pos.y+size.h), color);
}


void ag_surface32__draw_line(struct ag_surface32* surface, struct ag_vec2i start, struct ag_vec2i end, struct ag_color32 color)
{
	if(start.y == end.y)
	{
		if(start.x < end.x)
			ag_surface32__fill_rect(surface, start, ag_vec2i(end.x-start.x+1,1), color);
		else
			ag_surface32__fill_rect(surface, end, ag_vec2i(start.x-end.x+1,1), color);
		return;
	}

	if(start.x == end.x)
	{
		if(start.y < end.y)
			ag_surface32__fill_rect(surface, start, ag_vec2i(1,end.y-start.y+1), color);
		else
			ag_surface32__fill_rect(surface, end, ag_vec2i(1,start.y-end.y+1), color);
		return;
	}

	int dx = abs(end.x - start.x);
	int dy = abs(end.y - start.y);
	int sx = start.x < end.x ? 1 : -1;
	int sy = start.y < end.y ? 1 : -1;
	int err = (dx > dy ? dx : -dy)/2;
	int e2;

	for(;;)
	{
		surface->data[start.x+start.y*surface->size.w] = color;
		if(start.x == end.x && start.y == end.y)
			break;
		e2 = err;
		if(e2 > -dx)
		{
			err -= dy;
			start.x += sx;
		}

		if(e2 < dy)
		{
			err += dx;
			start.y += sy;
		}
	}
}



//todo: clean up mess
#ifdef __NO_INLINE__
#define inline
#endif 

inline void _dla_changebrightness(struct ag_color32* from, struct ag_color32* to, float br)
{
	if(br > 1.0)
		br = 1.0;
	to->r = br * (float)from->r + (1.0-br) * (float)to->r;
	to->g = br * (float)from->g + (1.0-br) * (float)to->g;
	to->b = br * (float)from->b + (1.0-br) * (float)to->b;
}

inline void _dla_plot(struct ag_surface32* img, int x, int y, struct ag_color32* col, float br)
{
  struct ag_color32 oc = img->data[x+y*img->size.w];
  _dla_changebrightness(col, &oc, br);
  img->data[x+y*img->size.w] = oc;
}
 
#define plot_(X,Y,D) do{ struct ag_color32 f_ = color;				\
  _dla_plot(img, (X), (Y), &f_, (D)) ; }while(0)
 
 
#define ipart_(X) ((int)(X))
#define round_(X) ((int)(((double)(X))+0.5))
#define fpart_(X) (((double)(X))-(double)ipart_(X))
#define rfpart_(X) (1.0-fpart_(X))
 
#define swap_(a, b) do{ __typeof__(a) tmp;  tmp = a; a = b; b = tmp; }while(0)

void ag_surface32__draw_line_aa(struct ag_surface32* img, struct ag_vec2i start, struct ag_vec2i end, struct ag_color32 color)
{
	int x1 = start.x, x2 = end.x, y1 = start.y, y2 = end.y;
  double dx = (double)x2 - (double)x1;
  double dy = (double)y2 - (double)y1;
  if ( fabs(dx) > fabs(dy) ) {
    if ( x2 < x1 ) {
      swap_(x1, x2);
      swap_(y1, y2);
    }
    double gradient = dy / dx;
    double xend = round_(x1);
    double yend = y1 + gradient*(xend - x1);
    double xgap = rfpart_(x1 + 0.5);
    int xpxl1 = xend;
    int ypxl1 = ipart_(yend);
    plot_(xpxl1, ypxl1, rfpart_(yend)*xgap);
    plot_(xpxl1, ypxl1+1, fpart_(yend)*xgap);
    double intery = yend + gradient;
 
    xend = round_(x2);
    yend = y2 + gradient*(xend - x2);
    xgap = fpart_(x2+0.5);
    int xpxl2 = xend;
    int ypxl2 = ipart_(yend);
    plot_(xpxl2, ypxl2, rfpart_(yend) * xgap);
    plot_(xpxl2, ypxl2 + 1, fpart_(yend) * xgap);
 
    int x;
    for(x=xpxl1+1; x <= (xpxl2-1); x++) {
      plot_(x, ipart_(intery), rfpart_(intery));
      plot_(x, ipart_(intery) + 1, fpart_(intery));
      intery += gradient;
    }
  } else {
    if ( y2 < y1 ) {
      swap_(x1, x2);
      swap_(y1, y2);
    }
    double gradient = dx / dy;
    double yend = round_(y1);
    double xend = x1 + gradient*(yend - y1);
    double ygap = rfpart_(y1 + 0.5);
    int ypxl1 = yend;
    int xpxl1 = ipart_(xend);
    plot_(xpxl1, ypxl1, rfpart_(xend)*ygap);
    plot_(xpxl1, ypxl1+1, fpart_(xend)*ygap);
    double interx = xend + gradient;
 
    yend = round_(y2);
    xend = x2 + gradient*(yend - y2);
    ygap = fpart_(y2+0.5);
    int ypxl2 = yend;
    int xpxl2 = ipart_(xend);
    plot_(xpxl2, ypxl2, rfpart_(xend) * ygap);
    plot_(xpxl2, ypxl2 + 1, fpart_(xend) * ygap);
 
    int y;
    for(y=ypxl1+1; y <= (ypxl2-1); y++) {
      plot_(ipart_(interx), y, rfpart_(interx));
      plot_(ipart_(interx) + 1, y, fpart_(interx));
      interx += gradient;
    }
  }
}
#undef swap_
#undef plot_
#undef ipart_
#undef fpart_
#undef round_
#undef rfpart_
