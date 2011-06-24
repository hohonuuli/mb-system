
/* Begin user code block <abstract> */
/*--------------------------------------------------------------------
 *    The MB-system:	mbeditviz_main.c		4/27/2007
 *    $Id$
 *
 *    Copyright (c) 2007-2011 by
 *    David W. Caress (caress@mbari.org)
 *      Monterey Bay Aquarium Research Institute
 *      Moss Landing, CA 95039
 *    and Dale N. Chayes (dale@ldeo.columbia.edu)
 *      Lamont-Doherty Earth Observatory
 *      Palisades, NY 10964
 *
 *    See README file for copying and redistribution conditions.
 *--------------------------------------------------------------------*/
/* End user code block <abstract> */

/**
 * README: Portions of this file are merged at file generation
 * time. Edits can be made *only* in between specified code blocks, look
 * for keywords <Begin user code> and <End user code>.
 */
/*
 * Generated by the ICS Builder Xcessory (BX).
 *
 * BuilderXcessory Version 6.1.3
 * Code Generator Xcessory 6.1.3 (08/19/04) CGX Scripts 6.1 Motif 2.1 
 *
 */


/* Begin user code block <file_comments> */
/* End user code block <file_comments> */

/*
 * Motif required Headers
 */
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/RepType.h>
#include <Xm/MwmUtil.h>

/**
 * Globally included information.
 */


/*
 * Headers for classes used in this program
 */

/**
 * Common constant and pixmap declarations.
 */
#include "mbeditviz_creation.h"

/**
 * Convenience functions from utilities file.
 */
extern void RegisterBxConverters(XtAppContext);
extern XtPointer BX_CONVERT(Widget, char *, char *, int, Boolean *);
extern XtPointer BX_DOUBLE(double);
extern XtPointer BX_SINGLE(float);
extern void BX_MENU_POST(Widget, XtPointer, XEvent *, Boolean *);
extern Pixmap XPM_PIXMAP(Widget, char **);
extern void BX_SET_BACKGROUND_COLOR(Widget, ArgList, Cardinal *, Pixel);

/**
 * Declarations for shell callbacks.
 */
extern void do_mbeditviz_quit(Widget, XtPointer, XtPointer);
extern void BxExitCB(Widget, XtPointer, XtPointer);

/* Begin user code block <globals> */

#include <stdio.h>
#include <stdlib.h>

/* MBIO include files */
#include "../../include/mb_status.h"
#include "../../include/mb_define.h"

/* mbview include file */
#include "mbview.h"

/* global mbeditviz variables */
#include "mbeditviz.h"

/* function prototypes */
Widget CreatemainWindow_mbeditviz(Widget parent);

/* use these parameters only when debugging X events */
#ifdef MBEDITVIZ_DEBUG    
char	eventname[64];
XEvent	event;
XAnyEvent *xany;
XKeyEvent *xkey;
XButtonEvent *xbutton;
XMotionEvent *xmotion;
XCrossingEvent *xcrossing;
XFocusChangeEvent *xfocus;
XExposeEvent *xexpose;
XGraphicsExposeEvent *xgraphicsexpose;
XNoExposeEvent *xnoexpose;
XVisibilityEvent *xvisibility;
XCreateWindowEvent *xcreatewindow;
XDestroyWindowEvent *xdestroywindow;
XUnmapEvent *xunmap;
XMapEvent *xmap;
XMapRequestEvent *xmaprequest;
XReparentEvent *xreparent;
XConfigureEvent *xconfigure;
XGravityEvent *xgravity;
XResizeRequestEvent *xresizerequest;
XConfigureRequestEvent *xconfigurerequest;
XCirculateEvent *xcirculate;
XCirculateRequestEvent *xcirculaterequest;
XPropertyEvent *xproperty;
XSelectionClearEvent *xselectionclear;
XSelectionRequestEvent *xselectionrequest;
XSelectionEvent *xselection;
XColormapEvent *xcolormap;
XClientMessageEvent *xclient;
XMappingEvent *xmapping;
XErrorEvent *xerror;
XKeymapEvent *xkeymap;
#endif

/* End user code block <globals> */

/**
 * Change this line via the Output Application Names Dialog.
 */
#define BX_APP_CLASS "MB-System"

int main( int argc, char **argv)
{
    Widget       parent;
    XtAppContext app;
    Arg          args[256];
    Cardinal     ac;
    Boolean      argok=False;
    Widget   topLevelShell;
    Widget   mainWindow_mbeditviz;
    
    /* Begin user code block <declarations> */
    
    int		argcsave;
    argcsave = argc;
    argc = 1;
        
    /* End user code block <declarations> */
    
    /*
     * Initialize Xt. 
     */
    
    XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL); 
    
    /*
     * The applicationShell is created as an unrealized
     * parent for multiple topLevelShells.  The topLevelShells
     * are created as popup children of the applicationShell.
     * This is a recommendation of Paul Asente & Ralph Swick in
     * _X_Window_System_Toolkit_ p. 677.
     */
    
    parent = XtVaOpenApplication ( &app, 
                                   BX_APP_CLASS, 
                                   NULL, 
                                   0, 
                                   &argc, 
                                   argv, 
                                   NULL, 
                                   sessionShellWidgetClass, 
                                   NULL );
    
    RegisterBxConverters(app);
    XmRepTypeInstallTearOffModelConverter();
    
    /* Begin user code block <create_shells> */
    /* End user code block <create_shells> */
    
    /*
     * Create classes and widgets used in this program. 
     */
    
    /* Begin user code block <create_topLevelShell> */
    /* End user code block <create_topLevelShell> */
    
    ac = 0;
    XtSetArg(args[ac], XmNtitle, "MBeditviz"); ac++;
    XtSetArg(args[ac], XmNx, 180); ac++;
    XtSetArg(args[ac], XmNy, 583); ac++;
    XtSetArg(args[ac], XmNwidth, 453); ac++;
    XtSetArg(args[ac], XmNheight, 557); ac++;
    topLevelShell = XtCreatePopupShell((char *)"topLevelShell",
        topLevelShellWidgetClass,
        parent,
        args, 
        ac);
    XtAddCallback(topLevelShell, XmNdestroyCallback, do_mbeditviz_quit, (XtPointer)0);
    XtAddCallback(topLevelShell, XmNdestroyCallback, BxExitCB, (XtPointer)0);
    mainWindow_mbeditviz = (Widget)CreatemainWindow_mbeditviz(topLevelShell);
    XtManageChild(mainWindow_mbeditviz);
    XtPopup(XtParent(mainWindow_mbeditviz), XtGrabNone);
    
    /* Begin user code block <app_procedures> */
    
    /* End user code block <app_procedures> */
    
    /* Begin user code block <main_loop> */
   
    /* initialize the gui code */
    do_mbeditviz_init(parent, app);
     
    /* initialize the vizualization widgets code */
    mbview_startup(mbev_verbose, parent, app, &mbev_error);
    mb3dsoundings_startup(mbev_verbose, parent, app, &mbev_error);
    mb3dsoundings_set_dismiss_notify(mbev_verbose, mbeditviz_mb3dsoundings_dismiss, &mbev_error);
    mb3dsoundings_set_edit_notify(mbev_verbose, mbeditviz_mb3dsoundings_edit, &mbev_error);
   
    /* initialize the program code */
    mbeditviz_init(argcsave,argv);
    
    /* End user code block <main_loop> */
    
    XtAppMainLoop(app);
    
    /*
     * A return value regardless of whether or not the main loop ends. 
     */
     return(0); 
}
