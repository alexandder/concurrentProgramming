#define _REENTRANT
#include <X11/Xlib.h>
#include <X11/X.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <X11/Xutil.h>

#define RECTSIZE 20
#define MAXPLAYERS 10

Display *mydisplay;
Window mywindow;
XSetWindowAttributes mywindowattributes;
XGCValues mygcvalues;
GC mygc;
Visual *myvisual;
int mydepth;
int myscreen;
Colormap mycolormap;
XColor dummy;
XColor redColor;
XColor blueColor;
XColor whiteColor;
XColor yellowColor;
XColor greenColor;
XEvent myevent;

pthread_t tid,tid1;
struct buffer {
	int x;
	int y;
	int pid;
	int prev;
	int isChasing;
	int end;
	int newRunnerPid;
};
int bufsize;
int p;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

typedef struct player {
  int pid;
  Region region;
  int isChasing;
  int x;
  int y;
  int lockMoves;
}PLAYER;

int startX1, startX2;
int startY1, startY2;
PLAYER players[MAXPLAYERS];
int playersNumber = 0;


int getPlayerIndex(PLAYER pl[], int playerN, int pid) {
	int i;
	for (i = 0; i < playerN; i++) {
		if (pl[i].pid == pid) {
			return i;
		}
	}
	return -1;
}

int getRunnerIndex(PLAYER pl[], int playerN) {
	int i;
	for (i = 0; i < playerN; i++) {
		if (pl[i].isChasing == 1) {
			return i;
		}
	}
	return -1;	
}

void *reader(void *argum) {
   struct buffer *buf;
   int xr,yr,xr1,yr1;
   int fd;

   buf=(struct buffer *) malloc(bufsize);
 
   while (!(fd=open("data",O_RDONLY,0700)));
		
		while (1) {  
  
 		if (read(fd,buf,bufsize)>0) {

 			if (buf->x > 530 || buf->x <= 0 || buf->y > 530 || buf->y <= 0) {
 				continue;
 			}
 			else {
        		if (((buf->pid==p)&&(argum!=NULL))||((buf->pid!=p)&&(argum==NULL))) {
		            pthread_mutex_lock(&lock);

		            // change colors
		            if (buf->newRunnerPid > 0 && buf->end != 1) {
		            	int oldIndex = getPlayerIndex(players, MAXPLAYERS, buf->pid);	

		            	if (players[oldIndex].lockMoves == 0) {
		            		
			            	int newIndex = getPlayerIndex(players, MAXPLAYERS, buf->newRunnerPid);
			            	players[oldIndex].isChasing = 0;
			            	players[newIndex].isChasing = 1;
			            	players[newIndex].lockMoves = RECTSIZE + 5;
		            		XClearWindow(mydisplay, mywindow);
		            		int j;
		               		for (j = 0; j < MAXPLAYERS; ++j) {
		               			if (players[j].pid > 0) {
			               			XSetForeground(mydisplay,mygc,blueColor.pixel);
			            			if (p == players[j].pid ) {
			            				XSetForeground(mydisplay,mygc,greenColor.pixel);
			            			}
			            			if (players[j].isChasing == 1) {
			            				XSetForeground(mydisplay,mygc,redColor.pixel);
			            			}
			            			XSetRegion(mydisplay, mygc, players[j].region);
			            			XFillRectangle(mydisplay,mywindow,mygc,players[j].x,
			               			players[j].y, players[j].x + RECTSIZE, players[j].y + RECTSIZE);	
			               		}
		              	 	}
			            	XFlush(mydisplay);
/*			               	printf("Change, nplayers: %d\n", playersNumber);
		            		for (j = 0; j < MAXPLAYERS; ++j) {
			               		if (players[j].pid > -1000) {
			               			printf("PID: %d\tchasing:%d\tlockMoves:%d\n",players[j].pid, players[j].isChasing, players[j].lockMoves);
			               		}
		               		}
		               	printf("\n");*/
			            }
		        	}

		            // Add new player
		            else if (getPlayerIndex(players, MAXPLAYERS, buf->pid) == -1 && buf->pid != -1 && buf->pid != 1 && buf->end != 1) {
		            	int j;
		            	int newIndex;
		            	for (j = 0; j < MAXPLAYERS; ++j) {
		            		if (players[j].pid < 1) {
		            			newIndex = j;
		            			break;
		            		}	     	
		            	}	     
		            	players[newIndex].pid = buf->pid;
		            	players[newIndex].x = buf->x;
		            	players[newIndex].y = buf->y;
		            	players[newIndex].lockMoves = 0;
		            	XPoint arr[4];

		            	arr[0].x = buf->x;
					    arr[0].y = buf->y;
					    arr[1].x = buf->x + RECTSIZE;
					    arr[1].y = buf->y;
					    arr[2].x = buf->x + RECTSIZE;
					    arr[2].y = buf->y + RECTSIZE;
					    arr[3].x = buf->x;
					    arr[3].y = buf->y + RECTSIZE;
		            	players[newIndex].region = XPolygonRegion(arr, 4, EvenOddRule);

		            	if (playersNumber == 0) {
		            		players[newIndex].isChasing = 1;
		            	}
		            	else {
		            		players[newIndex].isChasing = 0;	
		            	}
		            	playersNumber++;

		            	XClearWindow(mydisplay, mywindow);
		               	for (j = 0; j < MAXPLAYERS; ++j) {
		               		if (players[j].pid > 0) {
		               			XSetForeground(mydisplay,mygc,blueColor.pixel);
		            			if (p == players[j].pid ) {
		            				XSetForeground(mydisplay,mygc,greenColor.pixel);
		            			}
		            			if (players[j].isChasing == 1) {
		            				XSetForeground(mydisplay,mygc,redColor.pixel);
		            			}
		            			XSetRegion(mydisplay, mygc, players[j].region);
		            			XFillRectangle(mydisplay,mywindow,mygc,players[j].x,
		               			players[j].y, players[j].x + RECTSIZE, players[j].y + RECTSIZE);	
		               		}

		               	}
		            	XFlush(mydisplay);
/*		            	printf("Add, nplayers: %d\n", playersNumber);
		            	for (j = 0; j < MAXPLAYERS; ++j) {
			               	if (players[j].pid > -1000) {
			               		printf("PID: %d\tchasing:%d\tlockMoves:%d\n",players[j].pid, players[j].isChasing, players[j].lockMoves);
			               	}
		               	}
		               	printf("\n");*/
		            }
		            // Exit
		            else if (buf->end == 1) {
		            	int index = getPlayerIndex(players, MAXPLAYERS, buf->pid);
		            
		               	if (buf->newRunnerPid > 0) {
		               		int newInd = getPlayerIndex(players, MAXPLAYERS, buf->newRunnerPid);
		               		players[newInd].isChasing = 1;
		               	}
		               	players[index].pid = -1;
		               	players[index].isChasing = 0;
		               	players[index].lockMoves = 0;
		               	XClearWindow(mydisplay, mywindow);

		               	int j;
		               	for (j = 0; j < MAXPLAYERS; ++j) {
		               		if (players[j].pid > 0) {
		               			XSetForeground(mydisplay,mygc,blueColor.pixel);
		            			if (p == players[j].pid ) {
		            				XSetForeground(mydisplay,mygc,greenColor.pixel);
		            			}
		            			if (players[j].isChasing == 1) {
		            				XSetForeground(mydisplay,mygc,redColor.pixel);
		            			}
		            			XSetRegion(mydisplay, mygc, players[j].region);
		            			XFillRectangle(mydisplay,mywindow,mygc,players[j].x,
		               			players[j].y, players[j].x + RECTSIZE, players[j].y + RECTSIZE);	
		               		}

		               	}
		               	playersNumber--;
/*		               	printf("exit, nplayers: %d\n", playersNumber);
		               	for (j = 0; j < MAXPLAYERS; ++j) {
			               	if (players[j].pid > -1000) {
			               		printf("PID: %d\tchasing:%d\tlockMoves:%d\n",players[j].pid, players[j].isChasing, players[j].lockMoves);
			               	}
		               	}
		               	printf("\n");*/
		            	XFlush(mydisplay);

		            }
		            // Move player
		            else {
		            	int index = getPlayerIndex(players, MAXPLAYERS, buf->pid);
						XDestroyRegion(players[index].region);
						XPoint pt_arr[4];
			   			pt_arr[0].x = buf->x;
					    pt_arr[0].y = buf->y;
					    pt_arr[1].x = buf->x + RECTSIZE;
					    pt_arr[1].y = buf->y;
					    pt_arr[2].x = buf->x + RECTSIZE;
					    pt_arr[2].y = buf->y + RECTSIZE;
					    pt_arr[3].x = buf->x;
					    pt_arr[3].y = buf->y + RECTSIZE;
					    players[index].region = XPolygonRegion(pt_arr, 4, EvenOddRule);
					    XClearWindow(mydisplay, mywindow);
					    players[index].x = buf->x;
		               	players[index].y = buf->y;
		               	int j;
		               	for (j = 0; j < MAXPLAYERS; ++j) {
		               		if (players[j].pid > 0) {
		               			XSetForeground(mydisplay,mygc,blueColor.pixel);
		            			if (p == players[j].pid ) {
		            				XSetForeground(mydisplay,mygc,greenColor.pixel);
		            			}
		            			if (players[j].isChasing == 1) {
		            				XSetForeground(mydisplay,mygc,redColor.pixel);
		            			}
		            			XSetRegion(mydisplay, mygc, players[j].region);
		            			XFillRectangle(mydisplay,mywindow,mygc,players[j].x,
		               			players[j].y, players[j].x + RECTSIZE, players[j].y + RECTSIZE);	
		               		}

		               	}
					    XFlush(mydisplay);
		               	int runnerIndex = getRunnerIndex(players, MAXPLAYERS);
		               	if (players[runnerIndex].lockMoves > 0) {
		               		players[runnerIndex].lockMoves--;
		               	}
		            }
		            pthread_mutex_unlock(&lock);
		        }
		    }
     	}
   	}
}




int main(int argc, char const *argv[]) {
	
	p = getpid();
	XInitThreads();
  	int sed = time(NULL);
  	srand(sed);
  	startX1 = rand() % 500;
  	startY1 = rand() % 500;
    startX2 = startX1 + RECTSIZE;
    startY2 = startX1 + RECTSIZE;	

	mydisplay = XOpenDisplay("");
	myscreen = DefaultScreen(mydisplay);
	myvisual = DefaultVisual(mydisplay,myscreen);
	mydepth = DefaultDepth(mydisplay,myscreen);
	mywindowattributes.background_pixel = XWhitePixel(mydisplay,myscreen);
	mywindowattributes.override_redirect = False;

	mywindow = XCreateWindow(mydisplay,XRootWindow(mydisplay,myscreen),
	                    100,100,500,500,10,mydepth,InputOutput,
	                    myvisual,CWBackPixel|CWOverrideRedirect,
	                    &mywindowattributes);

	XSelectInput(mydisplay,mywindow,ExposureMask|KeyPressMask|ButtonPressMask|ButtonMotionMask);
	            
	mycolormap = DefaultColormap(mydisplay,myscreen);                

	XAllocNamedColor(mydisplay,mycolormap,"red",&redColor,&dummy);
	XAllocNamedColor(mydisplay,mycolormap,"blue",&blueColor,&dummy);
	XAllocNamedColor(mydisplay,mycolormap,"white",&whiteColor,&dummy);                  
	XAllocNamedColor(mydisplay,mycolormap,"green",&greenColor,&dummy);
	            
	XMapWindow(mydisplay,mywindow);

	mygc = DefaultGC(mydisplay,myscreen);

  	int xw,yw,xw1,yw1;
  	int fdw;

	struct buffer *bufw;

	bufsize=sizeof(struct buffer);

	bufw=(struct buffer *) malloc(bufsize);

	int start = 0;

	fdw=open("data",O_WRONLY|O_CREAT|O_APPEND,0700);
  
	pthread_create(&tid,NULL,reader,(void *) &p);
	pthread_create(&tid1,NULL,reader,NULL);


   	while (1) {

   		XNextEvent(mydisplay,&myevent);
   		switch (myevent.type) {

   			case Expose:
   				if (start == 0) {
		   			start = 1;
		   			bufw->x=startX1;
              		bufw->y=startY1;

              		bufw->prev=0;
              		bufw->pid=p;
              		bufw->isChasing = 1;
              		bufw->newRunnerPid = -1;
		   			write(fdw,bufw,bufsize);
		   			
	   			}
	            break;

		case ButtonPress:

	            xw1=myevent.xbutton.x; 
	            yw1=myevent.xbutton.y;
           		break;

           	case MotionNotify:
         
              	xw=myevent.xmotion.x;
             	yw=myevent.xmotion.y;
             	if (xw > 500 || xw <=0 || yw > 500 || yw <= 0) {
             		break;
             	}
	            bufw->x=xw;
              	bufw->y=yw;
              	bufw->pid=p;
              	bufw->newRunnerPid = -1;
              	bufw->end = 0;

              	int index = getPlayerIndex(players, MAXPLAYERS, p);
              	if (players[index].isChasing == 1 && players[index].lockMoves == 0) {
	               	int k;
		            for (k = 0; k < MAXPLAYERS; k++) {
		               	Region intersect = XCreateRegion();
		               	if (players[k].pid > 0 && players[k].pid != p) {
		               		XIntersectRegion(players[index].region, players[k].region, intersect);
		               		if (!(XEmptyRegion(intersect) == True)) {
								bufw->newRunnerPid = players[k].pid;
							    break;
		               		}
		               	}
		            }
	            }

	   			write(fdw,bufw,bufsize);
	   			
              	break;

            case KeyPress:
            	bufw->pid=p;
            	bufw->end = 1;
            	bufw->newRunnerPid = -1;
            	int cind = getPlayerIndex(players, MAXPLAYERS, p);
            	if (players[cind].isChasing == 1) {
            		int k;
            		for (k = 0; k < MAXPLAYERS; k++) {
            			if (players[k].pid > 0 && players[k].pid != p) {
            				bufw->newRunnerPid = players[k].pid;
            				break;
            			}
            		}
            	}
            	write(fdw,bufw,bufsize);
            	XCloseDisplay(mydisplay);
            	exit(0);
   		}
   }
   return 0;
}