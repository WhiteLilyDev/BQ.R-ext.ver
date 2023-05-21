
#ifndef GENKI_INFO_H
#define GENKI_INFO_H


#define VERSION_CODE 1
#define VERSION_STR "11.135.220"

unsigned int GenkiVersionCode() {
    return VERSION_CODE;
}

char *GenkiVersionStr() {
    return VERSION_STR;
}

#endif //GENKI_INFO_H
