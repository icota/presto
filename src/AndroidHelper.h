#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H

#include <QAndroidJniObject>
#include <jni.h>
#include <QDebug>

#ifdef __cplusplus
extern "C" {
#endif
    JNIEXPORT void JNICALL Java_com_codexapertus_presto_PrestoActivity_bolt11Received(JNIEnv *env, jobject obj, jstring Param1)
    {
        QAndroidJniObject bolt11String = QAndroidJniObject::fromLocalRef(Param1);
        qDebug() << "WE'RE IN" << bolt11String.toString();
    }
#ifdef __cplusplus
}
#endif

#endif // ANDROIDHELPER_H
