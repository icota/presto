#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H

#include <QAndroidJniObject>
#include <jni.h>
#include "LightningModel.h"
#include <QDebug>

extern "C" {
    JNIEXPORT void JNICALL Java_com_codexapertus_presto_PrestoActivity_bolt11Received(JNIEnv *env, jobject obj, jstring Param1)
    {
        Q_UNUSED(env);
        Q_UNUSED(obj);
        QAndroidJniObject bolt11String = QAndroidJniObject::fromLocalRef(Param1);
        qDebug() << "BOLT11 received from JNI: " << bolt11String.toString();
        LightningModel::instance()->paymentsModel()->decodePayment(bolt11String.toString());
    }
}

#endif // ANDROIDHELPER_H
