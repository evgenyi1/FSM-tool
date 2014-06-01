 graph 
 [
  node
   [
     id 0
     label "idle (d) "
 graphics
      [
       x   68     y   35   w   160  h   80  
       type	"roundrectangle"   hasFill	0 outline	"#000080"   outlineWidth	2 
      ] 
  
   ]
  node
   [
     id 1
     label "admin_down  "
 graphics
      [
       x   68     y   35   w   160  h   80  
       type	"roundrectangle"   hasFill	0 outline	"#008000"   outlineWidth	2 
      ] 
  
   ]
  node
   [
     id 2
     label "admin_up 
 tm 6000   "
 graphics
      [
       x   48     y   35   w   300  h   200
       type	"roundrectangle"   hasFill	0  outline	"#008000"
  outlineWidth	2    ] 
 LabelGraphics [anchor	"tr"] 
   isGroup 1
   ]
  node
   [
     id 3
     label "lag_down (d) "
 graphics
      [
       x   68     y   35   w   160  h   80  
       type	"roundrectangle"   hasFill	0 outline	"#000080"   outlineWidth	2 
      ] 
  
   gid 2
   ]
  node
   [
     id 4
     label "wait_lag_up 
 tm 2000   "
 graphics
      [
       x   68     y   35   w   160  h   80  
       type	"roundrectangle"   hasFill	0 outline	"#008000"   outlineWidth	2 
      ] 
  
   gid 2
   ]
  node
   [
     id 5
     label "lag_up  "
 graphics
      [
       x   68     y   35   w   160  h   80  
       type	"roundrectangle"   hasFill	0 outline	"#008000"   outlineWidth	2 
      ] 
  
   gid 2
   ]
  node
   [
     id 6
     label "condit1  "
 graphics
      [
       x   48     y   35   w   26  h   15
       type	"ellipse" fill	"#008000"    outline	"#008000"
  outlineWidth	2    ] 
 LabelGraphics [anchor	"tr"] 
   ]
  node
   [
     id 7
     label "mismatch  "
 graphics
      [
       x   68     y   35   w   160  h   80  
       type	"roundrectangle"   hasFill	0 outline	"#008000"   outlineWidth	2 
      ] 
  
   gid 2
   ]
  edge
   [
     source 0
     target 1
     label "tr00,ln51 : NULL " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "start_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 1
     target 2
     label "tr10,ln57 : on_admin_on (NULL) 

    /* SET_EVENT(lag_fsm , AdminOnEv) ; */
    /* lag->HandleAdminStatusChange(); */
    /* CalculateAlarm(LAG_ADMIN_UP); */
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "admin_on_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
 edge [ source	1 target 1 label " admin_down:

    printf(&quot;called %s\n&quot;, __FUNCTION__);
    return 0;
 " graphics [ 
 			     type	"arc" width	2 style	"dotted" fill	"#000000" arcType	"fixedRatio" arcHeight	-25.58 arcRatio	-0.76 ] 
                   LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free" ] 
 				  LabelGraphics [ text " "  fontSize 6   fontName  "Dialog"  model "side_slider" ]
 		          edgeAnchor [ xSource	-0.7284 ySource	-1.00031 xTarget	-0.7284 yTarget	1.00031 ] ] 
   edge
   [
     source 2
     target 2
     label "tr20,ln65 : NULL " 
  graphics  [ type	"arc"  width 1  fill   "#FF0000"  targetArrow	"standard" arcType	"fixedRatio" arcHeight	27.0 arcRatio	2.0 ] 
 edgeAnchor [ xSource	-0.4 ySource	-1  xTarget	0.5  yTarget	-1 ] 
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "port_down_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 2
     target 2
     label "tr21,ln66 : on_port_up_composed_state (NULL) 

    SET_EVENT(lag_fsm, port_up_ev);
    return 0;
 " 
  graphics  [ type	"arc"  width 1  fill   "#FF0000"  targetArrow	"standard" arcType	"fixedRatio" arcHeight	27.0 arcRatio	2.0 ] 
 edgeAnchor [ xSource	-0.4 ySource	-1  xTarget	0.5  yTarget	-1 ] 
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "port_up_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 2
     target 2
     label "tr22,ln67 : on_lacp_down_composed_state (NULL) 

    return 0;
 " 
  graphics  [ type	"arc"  width 1  fill   "#FF0000"  targetArrow	"standard" arcType	"fixedRatio" arcHeight	27.0 arcRatio	2.0 ] 
 edgeAnchor [ xSource	-0.4 ySource	-1  xTarget	0.5  yTarget	-1 ] 
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "lacp_down_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 2
     target 2
     label "tr23,ln68 : on_lacp_up_composed_state (NULL) 

    return 0;
 " 
  graphics  [ type	"arc"  width 1  fill   "#FF0000"  targetArrow	"standard" arcType	"fixedRatio" arcHeight	27.0 arcRatio	2.0 ] 
 edgeAnchor [ xSource	-0.4 ySource	-1  xTarget	0.5  yTarget	-1 ] 
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "lacp_up_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 2
     target 1
     label "tr24,ln69 : NULL " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "TIMER_EVENT  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 2
     target 1
     label "tr25,ln70 : on_admin_off (NULL) 

    /* lag->HandleAdminStatusChange(); */
    /* CalculateAlarm(LAG_ADMIN_DOWN); */
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "admin_off_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 3
     target 4
     label "tr30,ln77 : on_port_up_lacp_enable (NULL) 

    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "port_up_ev 
[ ev->lacp_enable ]" fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 3
     target 5
     label "tr31,ln78 : on_port_up_lacp_disable (NULL) 

    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "port_up_ev 
[ else ]" fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 3
     target 5
     label "tr32,ln79 : on_lacp_up_in_wait_state (NULL) 

    SET_EVENT(lag_fsm, lacp_up_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "lacp_up_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 3
     target 7
     label "tr33,ln80 : on_mismatch (NULL) 

    SET_EVENT(lag_fsm, mismatch_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "mismatch_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
 edge [ source	3 target 3 label " lag_down:

    printf(&quot;called %s\n&quot;, __FUNCTION__);
    return 0;
 " graphics [ 
 			     type	"arc" width	2 style	"dotted" fill	"#000000" arcType	"fixedRatio" arcHeight	-25.58 arcRatio	-0.76 ] 
                   LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free" ] 
 				  LabelGraphics [ text " "  fontSize 6   fontName  "Dialog"  model "side_slider" ]
 		          edgeAnchor [ xSource	-0.7284 ySource	-1.00031 xTarget	-0.7284 yTarget	1.00031 ] ] 
  edge [ source	3 target 3 label " lag_down:

    printf(&quot;called %s\n&quot;, __FUNCTION__);
    return 0;
 " graphics [ 
 			     type	"arc" width	2 style	"dotted" fill	"#000000" arcType	"fixedRatio" arcHeight	-25.58 arcRatio	 0.76 ] 
                   LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free" ] 
 				  LabelGraphics [ text " "  fontSize 6   fontName  "Dialog"  model "side_slider" ]
 		          edgeAnchor [ xSource	 0.7284 ySource	-1.00031 xTarget	 0.7284 yTarget	1.00031 ] ] 
   edge
   [
     source 4
     target 7
     label "tr40,ln89 : on_mismatch (NULL) 

    SET_EVENT(lag_fsm, mismatch_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "mismatch_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 4
     target 5
     label "tr41,ln90 : on_lacp_up_in_wait_state (NULL) 

    SET_EVENT(lag_fsm, lacp_up_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "lacp_up_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 4
     target 3
     label "tr42,ln91 : on_in_progress_tout (NULL) 

    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "TIMER_EVENT  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 5
     target 3
     label "tr50,ln97 : on_port_down_last_port (NULL) 

    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "port_down_ev 
[ fsm->lag == 0 ]" fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 5
     target 6
     label "tr51,ln98 : on_mismatch (NULL) 

    SET_EVENT(lag_fsm, mismatch_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "mismatch_ev  " fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 5
     target 3
     label "tr52,ln99 : on_lacp_down_last_port (NULL) 

    SET_EVENT(lag_fsm, lacp_down_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "lacp_down_ev 
[ fsm->lag  ]" fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 6
     target 7
     label "tr60,ln105 : on_mismatch (NULL) 

    SET_EVENT(lag_fsm, mismatch_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "  
[ fsm->lag == 0 ]" fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 6
     target 5
     label "tr61,ln106 : NULL " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "  
[ else ]" fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 7
     target 5
     label "tr70,ln112 : on_mismatch_cleared (1) 

    SET_EVENT(lag_fsm, lacp_down_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "lacp_down_ev 
[ fsm->lag ]" fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
  edge
   [
     source 7
     target 5
     label "tr71,ln113 : on_mismatch_cleared (0) 

    SET_EVENT(lag_fsm, lacp_down_ev);
    return 0;
 " 
  graphics  [ width	1  fill 	 "#FF0000"   targetArrow "standard" ] 
  
     LabelGraphics [ fontSize 0   fontName  "Dialog" visible 1  model "free"]
   LabelGraphics 
[
text "port_down_ev 
[ fsm->lag == 0 ]" fontSize 11   fontName  "Miriam"  model "free" ] 
   ]
 ]
