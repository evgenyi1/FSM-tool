FSM-tool
========

Tool for FSM development

Usage :
 fsm_generator  fsm_example   {path to .c fsm file}   {path to .h fsm file}
 
 files to be generated(updated) : fsm_example.c fsm_example.h 
 
 fsm generator: 
 1. finds fsm pseudo language code in  "C"  fsm file  (see example of fsm pseudo language code in fsm_example.c )
 2. parses pseudo language code and reports syntax errors if exist
 3. validates fsm code and reports validation error if need
 4. generates fsm code between placeholder symbols in the .c and .h files of fsm
 5. generates GML file (graphic modeling file which could be presented by Yed tool)


fsm_tool_framework.c/h - parts of user's code that runs FSM

fsm_tool_simulator - example how to init , start FSM and how to inject messages to FSM
