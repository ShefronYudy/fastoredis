#pragma once

/**/

#include <QDialog>
#include <QTableView>

#include "core/events/events_info.h"
#include "core/connection_types.h"

class QLabel;

namespace fastoredis
{
    class GlassWidget;
    class PropertyServerDialog
            : public QDialog
    {
        Q_OBJECT
    public:
        explicit PropertyServerDialog(const QString &title, connectionTypes type, QWidget *parent = 0);

    Q_SIGNALS:
        void showed();
        void changedProperty(const PropertyType &);

    public Q_SLOTS:
        void startServerProperty(const EventsInfo::ServerPropertyInfoRequest &req);
        void finishServerProperty(const EventsInfo::ServerPropertyInfoResponce &res);

        void startServerChangeProperty(const EventsInfo::ChangeServerPropertyInfoRequest &req);
        void finishServerChangeProperty(const EventsInfo::ChangeServerPropertyInfoResponce &res);

    protected:
        virtual void showEvent(QShowEvent *e);

    private:
        GlassWidget *glassWidget_;
        QTableView *propertyes_table_;
        const connectionTypes type_;       
    };
}
