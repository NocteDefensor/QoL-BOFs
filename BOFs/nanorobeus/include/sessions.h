#pragma once

#include <windows.h>
#include "bofdefs.h"
#include "common.h"
#include "luid.h"

void execute_sessions(WCHAR **dispatch, LUID luid, BOOL currentLuid);
NTSTATUS GetLogonSessionData(LUID luid, LOGON_SESSION_DATA *data);
void GetLogonTypeString(ULONG uLogonType, char **logonTypeString);
void PrintLogonSessionData(SECURITY_LOGON_SESSION_DATA data, char **sessionString);