#include <X11/Xlib.h>
#include <stdlib.h>//this is for malloc
//#include <stdio.h>
#include <math.h>
#include <time.h>

int width, height;
char* data;
struct timespec t0, t1;

void set (int x, int y, char r, char g, char b)
{
  char* p = data + y*width*4 + x*4;
  *p = b;p++;
  *p = g;p++;
  *p = r;
}

void clear()
{
  for(int i = 0; i < width; i++)
  {
    for(int j = 0; j < height; j++)
    {
      set(i,j,0,0,0);
    }
  }
}

void draw_circle(int x, int y, int radius)
{
  int circum = 2*M_PI*radius;
  
  for (int i = 0; i < circum; i++)
  {
    int penx = x + radius*sin((double)i/radius);
    int peny = y + radius*cos((double)i/radius);
    
    if (penx >= 0 && penx < width &&
	peny >= 0 && peny < height)
    {
      set(penx, peny, 255,255,255);
    }
  }
}

void setq (int x, int y, char r, char g, char b)
{
  int hs, vs;
  if (x < width-1)
    hs = 1;
  else
    hs = -1;
  if (y < height-1)
    vs = 1;
  else
    vs = -1;

  set(x,y,r,g,b);
  set(x+hs,y,r,g,b);
  set(x,y+vs,r,g,b);
  set(x+hs,y+vs,r,g,b);
}

int main()
{
  Display *display = XOpenDisplay(NULL);
  int screen_num = DefaultScreen(display);
  Window root = RootWindow(display,screen_num);
  Visual *visual = DefaultVisual(display,screen_num);

  width = 512;
  height = 512;

  data = (char *)malloc(width*height*4);

  XImage *img = XCreateImage(display,visual,24,ZPixmap,0,data,width,height,8,0);

  Window win = XCreateSimpleWindow(display,root,50,50,width,height,1,0,0);
  XSelectInput(display,win,ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask |
      ButtonPressMask);

  XMapWindow(display,win);

  clock_gettime(CLOCK_REALTIME,&t0);

  XEvent ev;
  int running = 1;
  while(running)
  {
    //clear();
    while(XPending(display))
    {
      XNextEvent(display,&ev);
      switch(ev.type)
      {
	case KeyPress:
	  running = 0;
	  break;
	case KeyRelease:
	  break;
	case MotionNotify:
	  if (ev.xmotion.state & Button1Mask)
	  {
	    setq(ev.xmotion.x,ev.xmotion.y,255,0,0);
	  }
	  else if (ev.xmotion.state & Button2Mask)
	  {
	    setq(ev.xmotion.x,ev.xmotion.y,0,255,0);
	  }
	  else if (ev.xmotion.state & Button3Mask)
	  {
	    setq(ev.xmotion.x,ev.xmotion.y,0,0,255);
	  }
	  else
	  {
	    draw_circle(ev.xmotion.x,ev.xmotion.y,10);
	    //setq(ev.xmotion.x,ev.xmotion.y,255,255,255);
	  }
	case ButtonPress:
	  switch (ev.xbutton.button)
	  {
	    case Button1:
	      draw_circle(ev.xmotion.x,ev.xmotion.y,10);
	      setq(ev.xbutton.x,ev.xbutton.y,255,127,127);
	      break;
	    case Button2:
	      setq(ev.xbutton.x,ev.xbutton.y,127,255,127);
              break;
	    case Button3: 
	      setq(ev.xbutton.x,ev.xbutton.y,127,127,255);
	      break;
	    case Button5:
	      setq(ev.xbutton.x,ev.xbutton.y,0,255,255);
	      break;
	  }
	  break;
      }

    }
    clock_gettime(CLOCK_REALTIME,&t1);
    int delta = (t1.tv_sec-t0.tv_sec)*1000000000+(t1.tv_nsec-t0.tv_nsec);
    if(delta >= 16666667)
    //        1000000000
    {
    XPutImage(display,win,DefaultGC(display,screen_num),img,0,0,0,0,width,height);
    clear();
    t0 = t1;
    }
  }
  XDestroyWindow(display,win);
  XCloseDisplay(display);
}
