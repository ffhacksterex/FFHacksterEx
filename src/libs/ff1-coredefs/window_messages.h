#pragma once

#define FFTTM_SHOWDESC     ((WM_APP) + 1000)
#define FFEDIT_RCLICK      ((WM_APP) + 1001)
#define FFEDIT_LCLICK      ((WM_APP) + 1002)

//TODO - does this need to be a registered windows message instead of WM_APP?
#define STR_RWM_FFHACKSTEREX_SHOWWAITMESSAGE  "RWM_FFHACKSTEREX_SHOWWAITMESSAGE"
extern UINT RWM_FFHACKSTEREX_SHOWWAITMESSAGE; // defined in general_functions.cpp

