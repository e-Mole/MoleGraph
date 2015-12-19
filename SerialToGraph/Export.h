#ifndef EXPORT_H
#define EXPORT_H

class Context;
class QString;
class Export
{
    Context const &m_context;
public:
    Export(const Context &context);

    void ToPng(QString const &fileName);
    void ToCsv(QString const &fileName);
};

#endif // EXPORT_H
