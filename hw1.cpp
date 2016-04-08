//Julia Acosta
//cs335 Spring 2016 hw-1
//
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
extern "C" {
    #include "fonts.h"
}

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 500
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
	float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};

struct Game {
	Shape box1, box2, box3, box4, box5;
	Particle particle[MAX_PARTICLES];
	int n;
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);


int main(void)
{
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//declare game object
	Game game;
	game.n=0;

	//declare box 1 shape
	game.box1.width = 100;
	game.box1.height = 10;
	game.box1.center.x = 120 + 1*65;
	game.box1.center.y = 500 - 1*60;

	//declare box 2 shape
        game.box2.width = 100;
        game.box2.height = 10;
        game.box2.center.x = 120 + 2*65;
        game.box2.center.y = 500 - 2*60;

	//declare box 3 shape
        game.box3.width = 100;
        game.box3.height = 10;
        game.box3.center.x = 120 + 3*65;
        game.box3.center.y = 500 - 3*60;

	//declare box 4 shape
        game.box4.width = 100;
        game.box4.height = 10;
        game.box4.center.x = 120 + 4*65;
        game.box4.center.y = 500 - 4*60;

	//declare box 5 shape
        game.box5.width = 100;
        game.box5.height = 10;
        game.box5.center.x = 120 + 5*65;
        game.box5.center.y = 500 - 5*60;

	//start animation
	while(!done) {
		while(XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_mouse(&e, &game);
			done = check_keys(&e, &game);
		}
		movement(&game);
		render(&game);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	cleanup_fonts();
	return 0;
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "335 hw1   LMB for particle");
}

void cleanupXWindows(void) {
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void) {
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if(vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
							ButtonPress | ButtonReleaseMask |
							PointerMotionMask |
							StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
					InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);
	
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}

void makeParticle(Game *game, int x, int y) {
	if (game->n >= MAX_PARTICLES)
		return;
	//std::cout << "makeParticle() " << x << " " << y << std::endl;
	//position of particle
	Particle *p = &game->particle[game->n];
	p->s.center.x = x;
	p->s.center.y = y;
	p->velocity.y = -1.0;
	p->velocity.x =  1.0;
	game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
	static int savex = 0;
	static int savey = 0;
	static int n = 0;

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed
			int y = WINDOW_HEIGHT - e->xbutton.y;
			makeParticle(game, e->xbutton.x, y);
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		if (++n < 10)
			return;
        //int y = WINDOW_HEIGHT - e->xbutton.y;
        //makeParticle(game, e->xbutton.x, y);

	}
}

int check_keys(XEvent *e, Game *game)
{
	//Was there input from the keyboard?
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		if (key == XK_Escape) {
			return 1;
		}
		//You may check other keys here.

	}
	switch(key) {
	    case XK_b:
		int y = WINDOW_HEIGHT - e->xbutton.y;
		makeParticle(game, e->xbutton.x, y);
		break;
	}
	return 0;
}

void movement(Game *game)
{
	Particle *p;

	if (game->n <= 0)
		return;

    for (int i=0; i<game->n; i++) {
	    p = &game->particle[i];
	    p->s.center.x += p->velocity.x;
	    p->s.center.y += p->velocity.y;

        //gravity
        p->velocity.y -= 0.2;

	    //check for collision with shapes...
	    Shape *s;
        s = &game->box1;
           if (p->s.center.y >= s->center.y - (s->height) &&
               p->s.center.y <= s->center.y + (s->height) &&
               p->s.center.x >= s->center.x - (s->width) &&
               p->s.center.x <= s->center.x + (s->width)) {
               p->velocity.y *= 0;
           }

	    //check for collision with shapes...
        s = &game->box2;
           if (p->s.center.y >= s->center.y - (s->height) &&
               p->s.center.y <= s->center.y + (s->height) &&
               p->s.center.x >= s->center.x - (s->width) &&
               p->s.center.x <= s->center.x + (s->width)) {
               p->velocity.y *= 0;
           }

            //check for collision with shapes...
        s = &game->box3;
           if (p->s.center.y >= s->center.y - (s->height) &&
               p->s.center.y <= s->center.y + (s->height) &&
               p->s.center.x >= s->center.x - (s->width) &&
               p->s.center.x <= s->center.x + (s->width)) {
               p->velocity.y *= 0;
           }

            //check for collision with shapes...
        s = &game->box4;
           if (p->s.center.y >= s->center.y - (s->height) &&
               p->s.center.y <= s->center.y + (s->height) &&
               p->s.center.x >= s->center.x - (s->width) &&
               p->s.center.x <= s->center.x + (s->width)) {
               p->velocity.y *= 0;
           }

            //check for collision with shapes...
        s = &game->box5;
           if (p->s.center.y >= s->center.y - (s->height) &&
               p->s.center.y <= s->center.y + (s->height) &&
               p->s.center.x >= s->center.x - (s->width) &&
               p->s.center.x <= s->center.x + (s->width)) {
               p->velocity.y *= 0;
           }


	    //check for off-screen
	    if (p->s.center.y < 0.0) {
		    std::cout << "off screen" << std::endl;
		    game->particle[i] = game->particle[game->n-1];
            game->n--;
	    }
    }
}

void render(Game *game)
{
	float w, h;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw shapes..
	Shape *s;
	//draw circle
	glColor3ub(90,140,90);
        s = &game->box1;
        glPushMatrix();
        glTranslatef(s->center.x, s->center.y, s->center.z);
        w = s->width;
        h = s->height;
//	glBegin(GL_LINE_LOOP);
//		for(int i =0; i <= 300; i++){
//		double angle = 2 * 3.14 * i / 300;
//		double x = Math.cos(angle);
//		double y = Math.sin(angle);
//		glVertex2d(x,y);
//		}
	glEnd(); 
	glPopMatrix();

	//draw box 1
	glColor3ub(90,140,90);
	s = &game->box1;
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, s->center.z);
	w = s->width;
	h = s->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();

	//draw box 2
        glColor3ub(90,140,90);
        s = &game->box2;
        glPushMatrix();
        glTranslatef(s->center.x, s->center.y, s->center.z);
        w = s->width;
        h = s->height;
        glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
        glEnd();
        glPopMatrix();

	//draw box 3
        glColor3ub(90,140,90);
        s = &game->box3;
        glPushMatrix();
        glTranslatef(s->center.x, s->center.y, s->center.z);
        w = s->width;
        h = s->height;
        glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
        glEnd();
        glPopMatrix();

	//draw box 4
        glColor3ub(90,140,90);
        s = &game->box4;
        glPushMatrix();
        glTranslatef(s->center.x, s->center.y, s->center.z);
        w = s->width;
        h = s->height;
        glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
        glEnd();
        glPopMatrix();

	//draw box 5
        glColor3ub(90,140,90);
        s = &game->box5;
        glPushMatrix();
        glTranslatef(s->center.x, s->center.y, s->center.z);
        w = s->width;
        h = s->height;
        glBegin(GL_QUADS);
                glVertex2i(-w,-h);
                glVertex2i(-w, h);
                glVertex2i( w, h);
                glVertex2i( w,-h);
        glEnd();
        glPopMatrix();


	//Print words

        Rect r;
        int yres = 600;

        r.bot = yres - 50;
        r.left = 10;
        r.center = 0;
        ggprint8b(&r, 20, 0x00FFFFFF, "Waterfall Model");

        //box1
        r.bot = yres - 165;
        r.left = 150;
        r.center = 0;
        ggprint8b(&r, 16, 0x00ff0000, "Requirements");

        //box2
        r.bot = yres - 225;
        r.left = 230;
        r.center = 0;
        ggprint8b(&r, 16, 0x00ff0000, "Design");

        //box3
        r.bot = yres - 285;
        r.left = 290;
        r.center = 0;
        ggprint8b(&r, 16, 0x00ff0000, "Coding");

        //box4
        r.bot = yres - 345;
        r.left = 360;
        r.center = 0;
        ggprint8b(&r, 16, 0x00ff0000, "Testing");

        //box5
        r.bot = yres - 405;
        r.left = 410;
        r.center = 0;
        ggprint8b(&r, 16, 0x00ff0000, "Maintenance");
	

	//draw all particles here
	glPushMatrix();
    for (int i=0; i<game->n; i++) {
	    glColor3ub(150,160,220);
	    Vec *c = &game->particle[i].s.center;
	    w = 2;
	    h = 2;
	    glBegin(GL_QUADS);
		    glVertex2i(c->x-w, c->y-h);
		    glVertex2i(c->x-w, c->y+h);
		    glVertex2i(c->x+w, c->y+h);
		    glVertex2i(c->x+w, c->y-h);
	    glEnd();
	    glPopMatrix();
    }
}



