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
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ScrolledW.h>
#include <Xm/List.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ScrolledW.h>
#include <Xm/List.h>
#include "MB3DRouteList.h"

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

/**
 * Declarations for callbacks and handlers.
 */
extern void do_mbview_routelist_delete(Widget, XtPointer, XtPointer);
extern void do_mbview_routelist_popdown(Widget, XtPointer, XtPointer);
extern void do_mbview_routelistselect(Widget, XtPointer, XtPointer);

/*
 * This table is used to define class resources that are placed
 * in app-defaults. This table is necessary so each instance
 * of this class has the proper default resource values set.
 * This eliminates the need for each instance to have
 * its own app-defaults values. This table must be NULL terminated.
 */
typedef struct _UIAppDefault {
    char*      cName;       /* Class name */
    char*      wName;       /* Widget name */
    char*      cInstName;   /* Name of class instance (nested class) */
    char*      wRsc;        /* Widget resource */
    char*      value;       /* value read from app-defaults */
} UIAppDefault;

static Boolean doInitAppDefaults = True;
static UIAppDefault  appDefaults[] = {
    {NULL, NULL, NULL, NULL, NULL}
};
/*
 * The functions to call in the apputils.c
 */
extern void InitAppDefaults(Widget, UIAppDefault*);
extern void SetAppDefaults(Widget, UIAppDefault*, char*, Boolean);

MB3DRouteListDataPtr
MB3DRouteListCreate ( MB3DRouteListDataPtr class_in, Widget parent, String name, ArgList args_in, Cardinal ac_in)
{
    Cardinal ac = 0;
    Arg      args[256];
    Cardinal cdc = 0;
    Boolean  argok = False;
    
    /**
     * Register the converters for the widgets.
     */
    RegisterBxConverters(XtWidgetToApplicationContext(parent));
    XtInitializeWidgetClass((WidgetClass)xmFormWidgetClass);
    XtInitializeWidgetClass((WidgetClass)xmLabelWidgetClass);
    XtInitializeWidgetClass((WidgetClass)xmPushButtonWidgetClass);
    XtInitializeWidgetClass((WidgetClass)xmScrolledWindowWidgetClass);
    XtInitializeWidgetClass((WidgetClass)xmListWidgetClass);
    /**
     * Setup app-defaults fallback table if not already done.
     */
    if (doInitAppDefaults)
    {
        InitAppDefaults(parent, appDefaults);
        doInitAppDefaults = False;
    }
    /**
     * Now set the app-defaults for this instance.
     */
    SetAppDefaults(parent, appDefaults, name, False);
    
    ac = 0;
    XtSetArg(args[ac], XmNresizePolicy, XmRESIZE_GROW); ac++;
    XtSetArg(args[ac], XmNx, 72); ac++;
    XtSetArg(args[ac], XmNy, 524); ac++;
    XtSetArg(args[ac], XmNwidth, 415); ac++;
    XtSetArg(args[ac], XmNheight, 239); ac++;
    class_in->MB3DRouteList = XmCreateForm(parent,
        (char *)name,
        args, 
        ac);
    
    ac = 0;
    {
        XmString    tmp0;
        
        tmp0 = (XmString) BX_CONVERT(class_in->MB3DRouteList, (char *)"Route List:", 
                XmRXmString, 0, &argok);
        XtSetArg(args[ac], XmNlabelString, tmp0); if (argok) ac++;
        XtSetArg(args[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
        XtSetArg(args[ac], XmNx, 10); ac++;
        XtSetArg(args[ac], XmNy, 10); ac++;
        XtSetArg(args[ac], XmNwidth, 390); ac++;
        XtSetArg(args[ac], XmNheight, 30); ac++;
        XtSetArg(args[ac], XmNfontList, 
            BX_CONVERT(class_in->MB3DRouteList, (char *)"-*-helvetica-bold-r-*-*-*-140-75-75-*-*-iso8859-1", 
            XmRFontList, 0, &argok)); if (argok) ac++;
        class_in->mbview_routelist_label = XmCreateLabel(class_in->MB3DRouteList,
            (char *)"mbview_routelist_label",
            args, 
            ac);
        XtManageChild(class_in->mbview_routelist_label);
        
        /**
         * Free any memory allocated for resources.
         */
        XmStringFree((XmString)tmp0);
    }
    
    
    ac = 0;
    {
        XmString    tmp0;
        
        tmp0 = (XmString) BX_CONVERT(class_in->MB3DRouteList, (char *)"Delete Selected Routes", 
                XmRXmString, 0, &argok);
        XtSetArg(args[ac], XmNlabelString, tmp0); if (argok) ac++;
        XtSetArg(args[ac], XmNx, 10); ac++;
        XtSetArg(args[ac], XmNy, 200); ac++;
        XtSetArg(args[ac], XmNwidth, 170); ac++;
        XtSetArg(args[ac], XmNheight, 30); ac++;
        XtSetArg(args[ac], XmNfontList, 
            BX_CONVERT(class_in->MB3DRouteList, (char *)"-*-helvetica-bold-r-*-*-*-140-75-75-*-*-iso8859-1", 
            XmRFontList, 0, &argok)); if (argok) ac++;
        class_in->mbview_pushButton_routelist_delete = XmCreatePushButton(class_in->MB3DRouteList,
            (char *)"mbview_pushButton_routelist_delete",
            args, 
            ac);
        XtManageChild(class_in->mbview_pushButton_routelist_delete);
        
        /**
         * Free any memory allocated for resources.
         */
        XmStringFree((XmString)tmp0);
    }
    
    XtAddCallback(class_in->mbview_pushButton_routelist_delete, XmNactivateCallback, do_mbview_routelist_delete, (XtPointer)0);
    
    ac = 0;
    {
        XmString    tmp0;
        
        tmp0 = (XmString) BX_CONVERT(class_in->MB3DRouteList, (char *)"Dismiss", 
                XmRXmString, 0, &argok);
        XtSetArg(args[ac], XmNlabelString, tmp0); if (argok) ac++;
        XtSetArg(args[ac], XmNx, 290); ac++;
        XtSetArg(args[ac], XmNy, 200); ac++;
        XtSetArg(args[ac], XmNwidth, 110); ac++;
        XtSetArg(args[ac], XmNheight, 30); ac++;
        XtSetArg(args[ac], XmNfontList, 
            BX_CONVERT(class_in->MB3DRouteList, (char *)"-*-helvetica-bold-r-*-*-*-140-75-75-*-*-iso8859-1", 
            XmRFontList, 0, &argok)); if (argok) ac++;
        class_in->mbview_pushButton_routelist_dismiss = XmCreatePushButton(class_in->MB3DRouteList,
            (char *)"mbview_pushButton_routelist_dismiss",
            args, 
            ac);
        XtManageChild(class_in->mbview_pushButton_routelist_dismiss);
        
        /**
         * Free any memory allocated for resources.
         */
        XmStringFree((XmString)tmp0);
    }
    
    XtAddCallback(class_in->mbview_pushButton_routelist_dismiss, XmNactivateCallback, do_mbview_routelist_popdown, (XtPointer)0);
    
    ac = 0;
    XtSetArg(args[ac], XmNscrollingPolicy, XmAPPLICATION_DEFINED); ac++;
    XtSetArg(args[ac], XmNx, 10); ac++;
    XtSetArg(args[ac], XmNy, 40); ac++;
    XtSetArg(args[ac], XmNwidth, 390); ac++;
    XtSetArg(args[ac], XmNheight, 150); ac++;
    class_in->mbview_scrolledWindow_routelist = XmCreateScrolledWindow(class_in->MB3DRouteList,
        (char *)"mbview_scrolledWindow_routelist",
        args, 
        ac);
    XtManageChild(class_in->mbview_scrolledWindow_routelist);
    
    ac = 0;
    XtSetArg(args[ac], XmNselectionPolicy, XmEXTENDED_SELECT); ac++;
    XtSetArg(args[ac], XmNwidth, 390); ac++;
    XtSetArg(args[ac], XmNheight, 150); ac++;
    XtSetArg(args[ac], XmNfontList, 
        BX_CONVERT(class_in->mbview_scrolledWindow_routelist, (char *)"-*-helvetica-bold-r-*-*-*-140-75-75-*-*-iso8859-1", 
        XmRFontList, 0, &argok)); if (argok) ac++;
    class_in->mbview_list_routelist = XmCreateList(class_in->mbview_scrolledWindow_routelist,
        (char *)"mbview_list_routelist",
        args, 
        ac);
    XtManageChild(class_in->mbview_list_routelist);
    XtAddCallback(class_in->mbview_list_routelist, XmNsingleSelectionCallback, do_mbview_routelistselect, (XtPointer)0);
    XtAddCallback(class_in->mbview_list_routelist, XmNextendedSelectionCallback, do_mbview_routelistselect, (XtPointer)0);
    XtAddCallback(class_in->mbview_list_routelist, XmNbrowseSelectionCallback, do_mbview_routelistselect, (XtPointer)0);
    ac = 0;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftOffset, 10); ac++;
    XtSetArg(args[ac], XmNrightOffset, 15); ac++;
    XtSetArg(args[ac], XmNtopOffset, 10); ac++;
    XtSetValues(class_in->mbview_routelist_label, args, ac);
    
    ac = 0;
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomOffset, 9); ac++;
    XtSetArg(args[ac], XmNleftOffset, 10); ac++;
    XtSetValues(class_in->mbview_pushButton_routelist_delete, args, ac);
    
    ac = 0;
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomOffset, 9); ac++;
    XtSetArg(args[ac], XmNrightOffset, 15); ac++;
    XtSetValues(class_in->mbview_pushButton_routelist_dismiss, args, ac);
    
    ac = 0;
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomOffset, 49); ac++;
    XtSetArg(args[ac], XmNleftOffset, 10); ac++;
    XtSetArg(args[ac], XmNrightOffset, 15); ac++;
    XtSetArg(args[ac], XmNtopOffset, 0); ac++;
    XtSetArg(args[ac], XmNtopWidget, class_in->mbview_routelist_label); ac++;
    XtSetValues(class_in->mbview_scrolledWindow_routelist, args, ac);
    
    
    /*
     * Assign functions to class record
     */
    
    
    /* Begin user code block <end_MB3DRouteListCreate> */
    /* End user code block <end_MB3DRouteListCreate> */
    
    return( class_in );
}
