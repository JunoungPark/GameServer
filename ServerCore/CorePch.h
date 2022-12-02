#pragma once


#include "Types.h"
#include "Macro.h"
#include "CoreTLS.h"
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <iostream>
using namespace std;

#include<mutex>
#include<atomic>

using LockGuard = lock_guard<mutex>;
using UniqLock = unique_lock<mutex>;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "ws2_32.lib")

void HelloWorld();