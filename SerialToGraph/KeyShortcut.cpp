#include "KeyShortcut.h"
#include <QShortcut>

KeyShortcut::KeyShortcut(QKeySequence const &sequence, QWidget *receiver, const char *slot):
    m_shortcut(NULL)
{
#if defined(Q_OS_ANDROID)
    //Android doesn't supprt keyboard shortcuts
    Q_UNUSED(sequence);
    Q_UNUSED(slot);
#else
    m_shortcut = new QShortcut(sequence, receiver);
    m_shortcut->connect(m_shortcut, SIGNAL(activated()), this, SIGNAL(activated()));
    //to be sender of this event KeyShortcut (not QShortcut)
    connect(this, SIGNAL(activated()), receiver, slot);
#endif
}

KeyShortcut::~KeyShortcut()
{
    delete m_shortcut;
}

QString KeyShortcut::GetText()
{
    return (m_shortcut == NULL ? "" : m_shortcut->key().toString());
}

QKeySequence KeyShortcut::GetKeySequence()
{
    return m_shortcut->key();
}
