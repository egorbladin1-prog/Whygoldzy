#define MAX_BP_LEVEL            100
#define BP_EXP_PER_LEVEL        1000
#define MAX_BP_TASKS            17
#define MAX_BP_TOP              15
#define BP_SEASON_NAME          "ТАЙНА ШКАТУЛКИ"

enum E_BP_DATA
{
    bP_ACCOUNT_ID,
    bpExp,
    bpLevel,
    bpPremium,
    bpTotalPoints,
    bpTasks[MAX_BP_TASKS],
    bpTaskProgress[MAX_BP_TASKS],
    bpRewards[MAX_BP_LEVEL] 
};

new PlayerBP[MAX_PLAYERS][E_BP_DATA];

enum E_BP_TOP
{
    toP_NAME[MAX_PLAYER_NAME],
    topPoints
};

new BlackPassTop[MAX_BP_TOP][E_BP_TOP];

stock GetPlayerBPExp(playerid) return PlayerBP[playerid][bpExp];
stock GetPlayerBPLevel(playerid) return PlayerBP[playerid][bpLevel];
stock GetPlayerBPStatus(playerid) return PlayerBP[playerid][bpPremium];

stock GivePlayerBPExp(playerid, amount)
{
    PlayerBP[playerid][bpExp] += amount;
    PlayerBP[playerid][bpTotalPoints] += amount;
    
    while(PlayerBP[playerid][bpExp] >= BP_EXP_PER_LEVEL && PlayerBP[playerid][bpLevel] < MAX_BP_LEVEL)
    {
        PlayerBP[playerid][bpExp] -= BP_EXP_PER_LEVEL;
        PlayerBP[playerid][bpLevel]++;
    }
    SavePlayerBP(playerid);
    UpdateBPLeaderboard(playerid);
    return 1;
}

stock ResetBlackPassSystem()
{
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(!IsPlayerConnected(i)) continue;
        ResetPlayerBP(i);
    }
    for(new j = 0; j < MAX_BP_TOP; j++)
    {
        BlackPassTop[j][topPoints] = 0;
        format(BlackPassTop[j][toP_NAME], MAX_PLAYER_NAME, "None");
    }
    mysql_tquery(mysql, "TRUNCATE TABLE `black_pass`", "", "");
    mysql_tquery(mysql, "TRUNCATE TABLE `black_pass_top`", "", "");
    return 1;
}

stock ResetPlayerBP(playerid)
{
    PlayerBP[playerid][bpExp] = 0;
    PlayerBP[playerid][bpLevel] = 1;
    PlayerBP[playerid][bpPremium] = 0;
    PlayerBP[playerid][bpTotalPoints] = 0;
    for(new i = 0; i < MAX_BP_TASKS; i++) 
    {
        PlayerBP[playerid][bpTasks][i] = 0;
        PlayerBP[playerid][bpTaskProgress][i] = 0;
    }
    return 1;
}

stock UpdateBPLeaderboard(playerid)
{
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, MAX_PLAYER_NAME);
    
    new currentPos = -1;
    for(new i = 0; i < MAX_BP_TOP; i++)
    {
        if(!strcmp(BlackPassTop[i][toP_NAME], name))
        {
            currentPos = i;
            break;
        }
    }
    
    if(currentPos != -1) BlackPassTop[currentPos][topPoints] = PlayerBP[playerid][bpTotalPoints];
    else if(PlayerBP[playerid][bpTotalPoints] > BlackPassTop[MAX_BP_TOP-1][topPoints])
    {
        format(BlackPassTop[MAX_BP_TOP-1][toP_NAME], MAX_PLAYER_NAME, name);
        BlackPassTop[MAX_BP_TOP-1][topPoints] = PlayerBP[playerid][bpTotalPoints];
    }
    
    for(new i = 0; i < MAX_BP_TOP; i++)
    {
        for(new j = i + 1; j < MAX_BP_TOP; j++)
        {
            if(BlackPassTop[j][topPoints] > BlackPassTop[i][topPoints])
            {
                new tempPoints = BlackPassTop[i][topPoints];
                new temP_NAME[MAX_PLAYER_NAME];
                format(temP_NAME, MAX_PLAYER_NAME, BlackPassTop[i][toP_NAME]);
                
                BlackPassTop[i][topPoints] = BlackPassTop[j][topPoints];
                format(BlackPassTop[i][toP_NAME], MAX_PLAYER_NAME, BlackPassTop[j][toP_NAME]);
                
                BlackPassTop[j][topPoints] = tempPoints;
                format(BlackPassTop[j][toP_NAME], MAX_PLAYER_NAME, temP_NAME);
            }
        }
    }
    return 1;
}

stock SavePlayerBP(playerid)
{
    new query[1024];
    mysql_format(mysql, query, sizeof(query), 
        "UPDATE `black_pass` SET `exp` = %d, `level` = %d, `premium` = %d, `total_points` = %d WHERE `user_id` = %d",
        PlayerBP[playerid][bpExp], PlayerBP[playerid][bpLevel], PlayerBP[playerid][bpPremium], PlayerBP[playerid][bpTotalPoints], g_player[playerid][P_ACCOUNT_ID]);
    mysql_tquery(mysql, query, "", "");
    return 1;
}

stock LoadPlayerBP(playerid)
{
    new query[128];
    mysql_format(mysql, query, sizeof(query), "SELECT * FROM `black_pass` WHERE `user_id` = %d", Player[playerid][P_ACCOUNT_ID]);
    mysql_tquery(mysql, query, "OnPlayerBPLoaded", "i", playerid);
    return 1;
}

forward OnPlayerBPLoaded(playerid);
public OnPlayerBPLoaded(playerid)
{
    new rows, fields;
    cache_get_data(rows, fields, mysql);
    if(rows > 0)
    {
        PlayerBP[playerid][bpExp] = cache_get_field_content_int(0, "exp", mysql);
        PlayerBP[playerid][bpLevel] = cache_get_field_content_int(0, "level", mysql);
        PlayerBP[playerid][bpPremium] = cache_get_field_content_int(0, "premium", mysql);
        PlayerBP[playerid][bpTotalPoints] = cache_get_field_content_int(0, "total_points", mysql);
    }
    else
    {
        new query[256];
        mysql_format(mysql, query, sizeof(query), 
            "INSERT INTO `black_pass` (`user_id`, `level`) VALUES (%d, 1)", g_player[playerid][P_ACCOUNT_ID]);
        mysql_tquery(mysql, query, "", "");
        ResetPlayerBP(playerid);
    }
    return 1;
}

forward LoadBPTop();
public LoadBPTop()
{
    new rows, fields;
    cache_get_data(rows, fields, mysql);
    for(new i = 0; i < rows && i < MAX_BP_TOP; i++)
    {
        BlackPassTop[i][topPoints] = cache_get_field_content_int(i, "total_points", mysql);
        format(BlackPassTop[i][toP_NAME], MAX_PLAYER_NAME, "ID: %d", cache_get_field_content_int(i, "user_id", mysql));
    }
    print("[Black Pass] Топ лидеров успешно загружен.");
    return 1;
}

CMD:lvlbp(playerid, params[])
{
    if(g_player[playerid][P_ADMIN] < 5)
        return ShowNotificationMisters(playerid, 2, 5, 1, 1, "Вам недоступна данная команда");
    

    new targetid, level;
    if(sscanf(params, "ui", targetid, level)) return SendClientMessage(playerid, -1, "{FF9900}Используйте: /lvlbp [id] [уровень]");
    if(!IsPlayerConnected(targetid)) return SendClientMessage(playerid, -1, "{FF0000}Игрок не в сети!");
    if(level < 1 || level > MAX_BP_LEVEL) return SendClientMessage(playerid, -1, "{FF0000}Неверный уровень!");

    PlayerBP[targetid][bpLevel] = level;
    PlayerBP[targetid][bpExp] = 0; 
    
    SavePlayerBP(targetid);
    
    new str[128];
    format(str, sizeof(str), "[A] Вы установили игроку %s %d уровень Black Pass.", g_player[targetid][P_NAME], level);
    SendClientMessage(playerid, -1, str);
    return 1;
}

CMD:expbp(playerid, params[])
{
    if(g_player[playerid][P_ADMIN] < 5)
        return ShowNotificationMisters(playerid, 2, 5, 1, 1, "Вам недоступна данная команда");
   

    new targetid, exp;
    if(sscanf(params, "ui", targetid, exp)) return SendClientMessage(playerid, -1, "{FF9900}Используйте: /expbp [id] [опыт]");
    if(!IsPlayerConnected(targetid)) return SendClientMessage(playerid, -1, "{FF0000}Игрок не в сети!");

    GivePlayerBPExp(targetid, exp);
    
    new str[128];
    format(str, sizeof(str), "[A] Вы выдали игроку %s %d опыта Black Pass.", g_player[targetid][P_NAME], exp);
    SendClientMessage(playerid, -1, str);
    return 1;
}

CMD:premiumbp(playerid, params[])
{
    if(g_player[playerid][P_ADMIN] < 5)
        return ShowNotificationMisters(playerid, 2, 5, 1, 1, "Вам недоступна данная команда");

    new targetid, status;
    if(sscanf(params, "ui", targetid, status)) return SendClientMessage(playerid, -1, "{FF9900}Используйте: /premiumbp [id] [0-1]");
    if(!IsPlayerConnected(targetid)) return SendClientMessage(playerid, -1, "{FF0000}Игрок не в сети!");
    if(status < 0 || status > 1) return SendClientMessage(playerid, -1, "{FF0000}Используйте 0 для снятия или 1 для выдачи!");

    PlayerBP[targetid][bpPremium] = status;
    SavePlayerBP(targetid);
    
    new str[128];
    format(str, sizeof(str), "[A] Вы %s статус PREMIUM для %s.", (status == 1 ? ("{66CC66}выдали") : ("{FF0000}забрали")), g_player[targetid][P_NAME]);
    SendClientMessage(playerid, -1, str);
    return 1;
}