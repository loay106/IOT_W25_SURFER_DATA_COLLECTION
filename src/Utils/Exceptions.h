#ifndef UNIT_EXCEPTIONS_H 
#define UNIT_EXCEPTIONS_H

using namespace std;
#include <exception>


class InitError : public exception {};
class SDCardError: public exception {};
class NotReadyError: public exception {};
class ESPNowSyncError: public exception {};


#endif /* UNIT_EXCEPTIONS_H */