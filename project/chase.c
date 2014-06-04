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
XColor mycolor,mycolor1,dummy;
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
}PLAYER;

PLAYER currentPlayer;
PLAYER players[MAXPLAYERS];
int playersNumber = 0;

int currentX1;
int currentY1;
int currentX2;
int currentY2;


int getPlayerIndex(PLAYER pl[], int playerN, int pid) {
	int i;
	for (i = 0; i < playerN; i++) {
		if (pl[i].pid == pid) {
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
     
        	//printf("Odczytano wspolrzedne: %5d, %5d\n",buf->x,buf->y);
        	//printf("PID: %d \t isChasing: %d \n",buf->pid, buf->isChasing);
        	//printf("playersNumber: %d\n", playersNumber);
         	//printf("pid= %5d p= %5d argum= %8d\n",buf->pid,p,argum);

 			if (buf->x > 530 || buf->x <= 0 || buf->y > 530 || buf->y <= 0) {
 				continue;
 			}
 			else {
        	if (((buf->pid==p)&&(argum!=NULL))||((buf->pid!=p)&&(argum==NULL))) {
				//printf("Probujemy zablokowac\n");
	            pthread_mutex_lock(&lock);


	            // change colors
	            if (buf->newRunnerPid > 0) {
	            	printf("Color change\n");
	            	int newIndex = getPlayerIndex(players, MAXPLAYERS, buf->newRunnerPid);
	            	int oldIndex = getPlayerIndex(players, MAXPLAYERS, buf->pid);
	            	players[oldIndex].isChasing = 0;
	            	players[newIndex].isChasing = 1;
	            	XSetForeground(mydisplay,mygc,redColor.pixel);
	            	XSetRegion(mydisplay, mygc, players[newIndex].region);
	               	XFillRectangle(mydisplay,mywindow,mygc,players[newIndex].x, players[newIndex].y, players[newIndex].x + RECTSIZE, players[newIndex].y + RECTSIZE);
	               	XFlush(mydisplay);
	               	XSetForeground(mydisplay,mygc,blueColor.pixel);
	            	if (p == players[oldIndex].pid ) {
	            		XSetForeground(mydisplay,mygc,greenColor.pixel);
	            	}
	            	XSetRegion(mydisplay, mygc, players[oldIndex].region);
	               	XFillRectangle(mydisplay,mywindow,mygc,players[oldIndex].x, players[oldIndex].y, players[oldIndex].x + RECTSIZE, players[oldIndex].y + RECTSIZE);
	               	XFlush(mydisplay);
	            }
	            // Add new player
	            else if (getPlayerIndex(players, MAXPLAYERS, buf->pid) == -1 && buf->pid != -1 && buf->pid != 1) {
	            	
	            	printf("Add new player\n");
	            	players[playersNumber].pid = buf->pid;
	            	players[playersNumber].x = buf->x;
	            	players[playersNumber].y = buf->y;
	            	XPoint arr[4];

	            	arr[0].x = buf->x;
				    arr[0].y = buf->y;
				    arr[1].x = buf->x + RECTSIZE;
				    arr[1].y = buf->y;
				    arr[2].x = buf->x + RECTSIZE;
				    arr[2].y = buf->y + RECTSIZE;
				    arr[3].x = buf->x;
				    arr[3].y = buf->y + RECTSIZE;
	            	players[playersNumber].region = XPolygonRegion(&arr, 4, EvenOddRule);

	            	if (playersNumber == 0) {
	            		players[playersNumber].isChasing = 1;
	            	}
	            	else {
	            		players[playersNumber].isChasing = 0;	
	            	}
	            	playersNumber++;
	            	int k;
	            	for (k = 0; k < playersNumber; k++) {
	            		printf("PID: %d\t X: %d \t Y: %d \t CH: %d\n", players[k].pid, players[k].x, players[k].y, players[k].isChasing);
	            		
	            	}
	            	printf("Current pid: %d\n", p);
	            	printf("playersNumber: %d\n", playersNumber);
	            }
	            // Exit
	            else if (buf->end == 1) {
	            	printf("Exit game\n");
	            	int index = getPlayerIndex(players, MAXPLAYERS, buf->pid);
	            	XSetForeground(mydisplay,mygc,whiteColor.pixel);
	            	XFillRectangle(mydisplay,mywindow,mygc,players[index].x, players[index].y, players[index].x + RECTSIZE, players[index].y + RECTSIZE);
	            	XFlush(mydisplay);
	            	playersNumber--;
	            }
	            // Move player
	            else {
	            	int index = getPlayerIndex(players, MAXPLAYERS, buf->pid);
	            	XSetForeground(mydisplay,mygc,whiteColor.pixel);
	            	//printf("read X: %d \t read Y: %d\n", players[index].x,players[index].y);	            
	            	XFillRectangle(mydisplay,mywindow,mygc,players[index].x, players[index].y, players[index].x + RECTSIZE, players[index].y + RECTSIZE);
	            	XSetForeground(mydisplay,mygc,blueColor.pixel);
	            	if (p == buf->pid ) {
	            		XSetForeground(mydisplay,mygc,greenColor.pixel);
	            	}
	            	if (players[index].isChasing == 1) {
	            		XSetForeground(mydisplay,mygc,redColor.pixel);
	            	}
	            	XOffsetRegion(players[index].region, buf->x - players[index].x, buf->y - players[index].y);
	               	XSetRegion(mydisplay, mygc, players[index].region);
	               	players[index].x = buf->x;
	               	players[index].y = buf->y;
	               	XFillRectangle(mydisplay,mywindow,mygc,players[index].x, players[index].y, players[index].x + RECTSIZE, players[index].y + RECTSIZE);
	               	XFlush(mydisplay);	               	
	            }
	            pthread_mutex_unlock(&lock);
	        }
	    }
     	}
   	}
}




int main(int argc, char const *argv[]) {
	
	p=getpid();
	XInitThreads();
  	int sed = time(NULL);
  	srand(sed);
  	currentX1 = rand() % 400;
  	currentY1 = rand() % 500;
    currentX2 = currentX1 + RECTSIZE;
    currentY2 = currentX1 + RECTSIZE;	

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
	                    
	XAllocNamedColor(mydisplay,mycolormap,"red",&mycolor,&dummy);
	            
	XAllocNamedColor(mydisplay,mycolormap,"blue",&mycolor1,&dummy);

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

	Region rect;
	Region rect2;

	int start = 0;
	int colorCheck = -1;

	fdw=open("data",O_WRONLY|O_CREAT|O_APPEND,0700);
  
	pthread_create(&tid,NULL,reader,(void *) &p);
	pthread_create(&tid1,NULL,reader,NULL);


   	while (1) {

   		XNextEvent(mydisplay,&myevent);
   		switch (myevent.type) {

   			case Expose:
   				if (start == 0) {
		   			printf("start\n");
		   			XSetForeground(mydisplay,mygc,greenColor.pixel);
		   			XPoint pt_arr[4];
		   			pt_arr[0].x = currentX1;
				    pt_arr[0].y = currentY1;
				    pt_arr[1].x = currentX1 + RECTSIZE;
				    pt_arr[1].y = currentY1;
				    pt_arr[2].x = currentX1 + RECTSIZE;
				    pt_arr[2].y = currentY1 + RECTSIZE;
				    pt_arr[3].x = currentX1;
				    pt_arr[3].y = currentY1 + RECTSIZE;

				    currentPlayer.pid = p;
				    currentPlayer.region = XPolygonRegion(&pt_arr, 4, EvenOddRule);
				    XSetRegion(mydisplay, mygc, currentPlayer.region);
				    XFillRectangle(mydisplay,mywindow,mygc,currentX1,currentY1,currentX2,currentY2);
				    XFlush(mydisplay);
		   			start = 1;
		   			bufw->x=currentX1;
              		bufw->y=currentY1;

              		bufw->prev=0;
              		bufw->pid=p;
              		bufw->isChasing = 1;
              		bufw->newRunnerPid = -1;
		   			write(fdw,bufw,bufsize);
		   			
		   			XDestroyRegion(currentPlayer.region);
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

              	int index = getPlayerIndex(players, playersNumber, p);
              	if (players[index].isChasing == 1) {
	               	int k;
		            for (k = 0; k < playersNumber; k++) {
		               	Region intersect = XCreateRegion();
		               	if (players[k].pid != p) {
		               		XIntersectRegion(players[index].region, players[k].region, intersect);
		               		if (!(XEmptyRegion(intersect) == True)) {
		               			printf("Found not-empty\n");
								bufw->newRunnerPid = players[k].pid;
							    break;
		               		}
		               	}
		            }
	            }

	   			write(fdw,bufw,bufsize);
	   			
              	break;

            case KeyPress:
            	printf("End\n");
            	playersNumber--;
            	bufw->pid=p;
            	bufw->end = 1;
            	bufw->newRunnerPid = -1;
            	write(fdw,bufw,bufsize);
     
            	XCloseDisplay(mydisplay);
            	exit(0);
   		}
   }

   return 0;
}