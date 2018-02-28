#include "Settings.h"

bool Settings::menuopen = false;
bool Settings::input_shouldListen = false;
ButtonCode_t* Settings::input_receivedKeyval = nullptr;
//ButtonCode_t Settings::legit_aimkey = BUTTON_CODE_NONE;
//ButtonCode_t Settings::legit_aimkey2 = BUTTON_CODE_NONE;
int Settings::legit_aimkey = 0;
int Settings::legit_aimkey2 = 0;