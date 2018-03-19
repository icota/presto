#include "3rdparty/linux_libnfc-nci/src/include/linux_nfc_api.h"

unsigned char APP_SELECT[] = {0x00,0xA4,0x04,0x00,0x07,0xD2,0x76,0x00,0x00,0x85,0x01,0x01};
unsigned char CC_SELECT[] = {0x00,0xA4,0x00,0x0C,0x02,0xE1,0x03};
unsigned char NDEF_READ[] = {0x00,0xB0};
unsigned char REPLY_NOK[] = {0x6A, 0x82};

static nfcTagCallback_t nfcTagCallback;

nfc_tag_info_t currentTagInfo;
bool tagPresent = false;

void onTagArrival(nfc_tag_info_t *pTagInfo)
{
    qDebug() << "NFC tag is present!";
    memcpy(&currentTagInfo, pTagInfo, sizeof(nfc_tag_info_t));
    tagPresent = true;
}

void onTagDeparture(void)
{
    qDebug() << "NFC tag is gone!";
    tagPresent = false;
}

void initializeNfc()
{
    if (nfcManager_isNfcActive()) qDebug() << "Error: NFC is already active";

    if (nfcManager_doInitialize() == 0) {
        nfcTagCallback.onTagArrival = onTagArrival;
        nfcTagCallback.onTagDeparture = onTagDeparture;
        nfcManager_registerTagCallback(&nfcTagCallback);

        qDebug() << "Enabling NFC discovery...";
        nfcManager_enableDiscovery(DEFAULT_NFA_TECH_MASK, 0, 0, 1);
    }

    if (nfcManager_isNfcActive()) qDebug() << "NFC is active!";
}
