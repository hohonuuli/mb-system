/* Added HAVE_CONFIG_H for autogen files */
#ifdef HAVE_CONFIG_H
#  include <mbsystem_config.h>
#endif



/* Begin user code block <abstract> */
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
#include "MB3DSiteList.h"
#include "MB3DRouteList.h"
#include "MB3DNavList.h"
#include "MB3DView.h"

/**
 * Common constant and pixmap declarations.
 */
#include "creation-c.h"

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

/* Begin user code block <globals> */
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
    Widget   mainWindow_mbgrdviz;
    
    /* Begin user code block <declarations> */
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
    XtSetArg(args[ac], XmNtitle, "MBgrdviz"); ac++;
    XtSetArg(args[ac], XmNx, 119); ac++;
    XtSetArg(args[ac], XmNy, 214); ac++;
    XtSetArg(args[ac], XmNwidth, 197); ac++;
    XtSetArg(args[ac], XmNheight, 104); ac++;
    topLevelShell = XtCreatePopupShell((char *)"topLevelShell",
        topLevelShellWidgetClass,
        parent,
        args, 
        ac);
    mainWindow_mbgrdviz = (Widget)CreatemainWindow_mbgrdviz(topLevelShell);
    XtManageChild(mainWindow_mbgrdviz);
    XtPopup(XtParent(mainWindow_mbgrdviz), XtGrabNone);
    
    /* Begin user code block <app_procedures> */
    /* End user code block <app_procedures> */
    
    /* Begin user code block <main_loop> */
    /* End user code block <main_loop> */
    
    XtAppMainLoop(app);
    
    /*
     * A return value regardless of whether or not the main loop ends. 
     */
     return(0); 
}
