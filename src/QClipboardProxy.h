#ifndef QCLIPBOARDPROXY_H
#define QCLIPBOARDPROXY_H

#include <QObject>

class QClipboard;

class QClipboardProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)

public:
    explicit QClipboardProxy(QClipboard* clipboard);
    QString text() const;

public slots:
    void setText(QString text);

private slots:
    void dataChanged();

signals:
    void textChanged();

private:
    QClipboard* m_clipboard;
};

#endif // QCLIPBOARDPROXY_H
