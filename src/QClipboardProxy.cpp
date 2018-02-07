#include "QClipboardProxy.h"
#include <QClipboard>

QClipboardProxy::QClipboardProxy(QClipboard* clipboard) : m_clipboard(clipboard)
{
    // TODO: fix
    //QObject::connect(m_clipboard, &QClipboard::dataChanged, this, QClipboardProxy::dataChanged);
}

void QClipboardProxy::setText(QString text)
{
    m_clipboard->setText(text);
}

void QClipboardProxy::dataChanged()
{
    emit textChanged();
}

QString QClipboardProxy::text() const
{
    return m_clipboard->text();
}
