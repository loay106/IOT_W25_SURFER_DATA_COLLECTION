#ifndef UNIT_EXCEPTIONS_H 
#define UNIT_EXCEPTIONS_H

using namespace std;
#include <exception>

class BaseError: public exception {};
class InitError : public BaseError {};
class SDCardError: public BaseError {};
class NotReadyError: public BaseError {};
class ESPNowSyncError: public BaseError {};
class InvalidSyncMessage: public BaseError {};


#endif /* UNIT_EXCEPTIONS_H */