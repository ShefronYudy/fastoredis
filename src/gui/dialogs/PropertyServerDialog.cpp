#include "gui/dialogs/PropertyServerDialog.h"

#include <QLabel>
#include <QHBoxLayout>

#include "common/qt/converter_patterns.h"
#include "gui/GuiFactory.h"
#include "gui/GlassWidget.h"
#include "gui/PropertyTableModel.h"

namespace fastoredis
{
    PropertyServerDialog::PropertyServerDialog(const QString &title, connectionTypes type, QWidget *parent)
        : QDialog(parent), type_(type)
    {
        setWindowTitle(title);
        propertyes_table_ = new QTableView(this);
        PropertyTableModel *mod = new PropertyTableModel(propertyes_table_);
        VERIFY(connect(mod, SIGNAL(changedProperty(const PropertyType &)), this, SIGNAL(changedProperty(const PropertyType &))));
        propertyes_table_->setModel(mod);

        QHBoxLayout *mainL = new QHBoxLayout(this);
        mainL->addWidget(propertyes_table_);
        setLayout(mainL);

        glassWidget_ = new GlassWidget(GuiFactory::instance().loadingPathFilePath(), "Loading...", 0.5, QColor(111, 111, 100), this);
    }

    void PropertyServerDialog::startServerProperty(const EventsInfo::ServerPropertyRequest &req)
    {
        glassWidget_->start();
    }

    void PropertyServerDialog::finishServerProperty(const EventsInfo::ServerPropertyResponce &res)
    {
        glassWidget_->stop();
        common::ErrorValue er = res.errorInfo();
        if(!er.isError()){
            if(type_ == REDIS){
                ServerPropertyInfo inf = res.info_;
                PropertyTableModel *model = qobject_cast<PropertyTableModel*>(propertyes_table_->model());
                for(int i = 0; i < inf.propertyes_.size(); ++i)
                {
                    PropertyType it = inf.propertyes_[i];
                    model->addItem(new PropertyTableItem(common::utils_qt::toQString(it.first), common::utils_qt::toQString(it.second)));
                }
            }
        }
    }

    void PropertyServerDialog::startServerChangeProperty(const EventsInfo::ServerPropertyChangeRequest &req)
    {

    }

    void PropertyServerDialog::finishServerChangeProperty(const EventsInfo::ServerPropertyChangeResponce &res)
    {
        common::ErrorValue er = res.errorInfo();
        if(!er.isError()){
            if(type_ == REDIS){
                PropertyType pr = res.newItem_;
                if(res.isChange_){
                    PropertyTableModel *model = qobject_cast<PropertyTableModel*>(propertyes_table_->model());
                    model->changeProperty(pr);
                }
            }
        }
    }

    void PropertyServerDialog::showEvent(QShowEvent *e)
    {
        QDialog::showEvent(e);
        emit showed();
    }
}
