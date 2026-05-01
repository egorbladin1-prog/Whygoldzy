#if defined _casino_fix_included
    #endinput
#endif
#define _casino_fix_included

#include <a_samp>

public OnGameModeInit()
{
    // маркер выхода
    CreateDynamicPickup(1318, 23, 2498.7519, 2511.5380, 2501.0791, -1);
    
    // нпс крупье
    CreateActor(171, 2516.3137, 2489.1713, 2501.1181, 180.0);
    
    #if defined casino_OnGameModeInit
        return casino_OnGameModeInit();
    #else
        return 1;
    #endif
}
#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit casino_OnGameModeInit
#if defined casino_OnGameModeInit
    forward casino_OnGameModeInit();
#endif

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if((newkeys & KEY_WALK) || (newkeys & KEY_YES) || (newkeys & KEY_SECONDARY_ATTACK))
    {
        if(IsPlayerInRangeOfPoint(playerid, 2.0, 2516.3137, 2489.1713, 2501.1181))
        {
            ShowPlayerDialog(playerid, 8888, DIALOG_STYLE_LIST, "Работа крупье", "1. Устроиться / Переодеться\n2. Закончить работу", "Выбрать", "Закрыть");
        }
    }
    
    #if defined casino_OnPlayerKeyStateCh
        return casino_OnPlayerKeyStateCh(playerid, newkeys, oldkeys);
    #else
        return 1;
    #endif
}
#if defined _ALS_OnPlayerKeyStateChange
    #undef OnPlayerKeyStateChange
#else
    #define _ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange casino_OnPlayerKeyStateCh
#if defined casino_OnPlayerKeyStateCh
    forward casino_OnPlayerKeyStateCh(playerid, newkeys, oldkeys);
#endif

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if(dialogid == 8888)
    {
        if(response)
        {
            if(listitem == 0)
            {
                SetPlayerSkin(playerid, 171);
                SendClientMessage(playerid, -1, "ты переоделся и начал смену");
            }
            else if(listitem == 1)
            {
                SendClientMessage(playerid, -1, "ты закончил работу");
            }
        }
        return 1;
    }
    
    #if defined casino_OnDialogResponse
        return casino_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
    #else
        return 0;
    #endif
}
#if defined _ALS_OnDialogResponse
    #undef OnDialogResponse
#else
    #define _ALS_OnDialogResponse
#endif
#define OnDialogResponse casino_OnDialogResponse
#if defined casino_OnDialogResponse
    forward casino_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif

CMD:cexit(playerid, params[])
{
    if(IsPlayerInRangeOfPoint(playerid, 3.0, 2498.7519, 2511.5380, 2501.0791))
    {
        SetPlayerPos(playerid, 1333.5756, 2368.6416, 17.7120);
        SetPlayerFacingAngle(playerid, 178.0147);
        SetPlayerInterior(playerid, 0); 
        SetPlayerVirtualWorld(playerid, 0); 
        SetCameraBehindPlayer(playerid);
        return 1;
    }
    SendClientMessage(playerid, -1, "ты не у выхода!");
    return 1;
}