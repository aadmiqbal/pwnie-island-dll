#include <dlfcn.h>
#include <set>
#include <map>
#include <functional>
#include <string>
#include <vector>
#include "libGameLogic.h"

void Player::SetJumpState(bool b) 
{
    printf("[*] SetJumpState(%d)\n", b);
}
