// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2024 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  client_connection.h
/// \brief Client connection handling

#ifndef __D_CLIENT_CONNECTION__
#define __D_CLIENT_CONNECTION__

#include "../doomtype.h"
#include "d_clisrv.h"

#define MAXSERVERLIST (MAXNETNODES-1)

typedef struct
{
	SINT8 node;
	serverinfo_pak info;
} serverelem_t;

typedef enum
{
	CL_SEARCHING,
	CL_CHECKFILES,
	CL_DOWNLOADFILES,
	CL_ASKJOIN,
	CL_LOADFILES,
	CL_WAITJOINRESPONSE,
	CL_DOWNLOADSAVEGAME,
	CL_CONNECTED,
	CL_ABORTED,
	CL_ASKFULLFILELIST,
	CL_CONFIRMCONNECT,
	CL_DOWNLOADHTTPFILES,
	CL_VIEWSERVER
} cl_mode_t;

extern serverelem_t serverlist[MAXSERVERLIST];
extern UINT32 serverlistcount;

extern cl_mode_t cl_mode;
extern boolean serverisfull; //lets us be aware if the server was full after we check files, but before downloading, so we can ask if the user still wants to download or not
extern tic_t firstconnectattempttime;
extern UINT8 mynode; // my address pointofview server
extern SINT8 splitservernode;
extern UINT8 splitmynode;
boolean CL_IsTestClientNode(INT32 node);
boolean CL_IsTestClientPlayer(INT32 playernum);
void CL_MarkTestClientPlayer(INT32 servernode, INT32 playernum);
void CL_ClearTestClientPlayer(INT32 playernum);
void CL_TestClientsChanged(void);
void CL_TestClientsTicker(void);
void CL_ResetTestClients(void);
void CL_TestClientNodeAccepted(SINT8 node, UINT8 servernode);
void CL_TestClientConnectionLost(SINT8 node);
void CL_SendTestClientKeepAlives(void);
void CL_SendTestClientCommands(void);

void CL_QueryServerList(msg_server_t *list);
void CL_UpdateServerList(boolean internetsearch, INT32 room);

void CL_ConnectToServer(void);
boolean CL_SendJoin(void);
boolean CL_StartSplitConnection(void);
void CL_ResetSplitConnection(void);
void CL_SplitConnectionLost(void);

void PT_ServerInfo(SINT8 node);
void PT_PlayerInfo(SINT8 node);
void PT_MoreFilesNeeded(SINT8 node);
void PT_ServerRefuse(SINT8 node);
void PT_ServerCFG(SINT8 node);

#endif
