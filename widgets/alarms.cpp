/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2017 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include <QDesktopServices>
#include <QDialog>
#include <QtScript>

#include <parameter.h>
#include <xsqlquery.h>

#include "alarmMaint.h"
#include "alarms.h"


// CAUTION: This will break if the order of this list does not match
//          the order of the enumerated values as defined.
const Alarms::AlarmMap Alarms::_alarmMap[] =
{
  AlarmMap( Uninitialized,     " "   ),
  AlarmMap( Address,           "ADDR"),
  AlarmMap( BBOMHead,          "BBH" ),
  AlarmMap( BBOMItem,          "BBI" ),
  AlarmMap( BOMHead,           "BMH" ),
  AlarmMap( BOMItem,           "BMI" ),
  AlarmMap( BOOHead,           "BOH" ),
  AlarmMap( BOOItem,           "BOI" ),
  AlarmMap( CRMAccount,        "CRMA"),
  AlarmMap( Contact,           "T"   ),
  AlarmMap( Customer,          "C"   ),
  AlarmMap( Employee,          "EMP" ),
  AlarmMap( Incident,          "INCDT"),
  AlarmMap( Item,              "I"   ),
  AlarmMap( ItemSite,          "IS"  ),
  AlarmMap( ItemSource,        "IR"  ),
  AlarmMap( Location,          "L"   ),
  AlarmMap( LotSerial,         "LS"   ),
  AlarmMap( Opportunity,       "OPP" ),
  AlarmMap( Project,           "J"   ),
  AlarmMap( PurchaseOrder,     "P"   ),
  AlarmMap( PurchaseOrderItem, "PI"  ),
  AlarmMap( ReturnAuth,        "RA"  ),
  AlarmMap( ReturnAuthItem,    "RI"  ),
  AlarmMap( Quote,             "Q"   ),
  AlarmMap( QuoteItem,         "QI"  ),
  AlarmMap( SalesOrder,        "S"   ),
  AlarmMap( SalesOrderItem,    "SI"  ),
  AlarmMap( TodoItem,          "TODO" ),
  AlarmMap( TransferOrder,     "TO"  ),
  AlarmMap( TransferOrderItem, "TI"  ),
  AlarmMap( Vendor,            "V"   ),
  AlarmMap( Warehouse,         "WH"  ),
  AlarmMap( WorkOrder,         "W"   ),
};

Alarms::Alarms(QWidget *pParent) :
  QWidget(pParent)
{
  setupUi(this);
  
  _source = Uninitialized;
  _sourceid = -1;
  _usrId1 = -1;
  _usrId2 = -1;
  _usrId3 = -1;
  _cntctId1 = -1;
  _cntctId2 = -1;
  _cntctId3 = -1;
  _readOnly = false;
  XSqlQuery tickle;
  if(_x_metrics)
  {
    tickle.exec( "SELECT CURRENT_TIME AS dbdate;" );
    if (tickle.first())
    {
      _dueDate = tickle.value("dbdate").toDate();
      _dueTime = tickle.value("dbdate").toTime();
    }
  }


  _alarms->addColumn(tr("Qualifier"),        _itemColumn,   Qt::AlignLeft, true, "f_offset" );
  _alarms->addColumn(tr("Due"),              -1,            Qt::AlignLeft, true, "alarm_time" );
  _alarms->addColumn(tr("Event Recipient"),  _itemColumn,   Qt::AlignLeft, true, "alarm_event_recipient" );
  _alarms->addColumn(tr("SysMsg Recipient"), _itemColumn,   Qt::AlignLeft, true, "alarm_sysmsg_recipient" );
  if (_x_metrics)
  {
    if (_x_metrics->boolean("EnableBatchManager"))
      _alarms->addColumn(tr("Email Recipient"),  _itemColumn,   Qt::AlignLeft, true, "alarm_email_recipient" );
  }

  connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));
  connect(_edit, SIGNAL(clicked()), this, SLOT(sEdit()));
  connect(_view, SIGNAL(clicked()), this, SLOT(sView()));
  connect(_delete, SIGNAL(clicked()), this, SLOT(sDelete()));
}

void Alarms::setType(enum AlarmSources pSource)
{
  _source = pSource;
}

void Alarms::setId(int pSourceid)
{
  _sourceid = pSourceid;
  refresh();
}

void Alarms::setUsrId1(int pUsrId)
{
  _usrId1 = pUsrId;
}

void Alarms::setUsrId2(int pUsrId)
{
  _usrId2 = pUsrId;
}

void Alarms::setUsrId3(int pUsrId)
{
  _usrId3 = pUsrId;
}

void Alarms::setCntctId1(int pCntctId)
{
  _cntctId1 = pCntctId;
}

void Alarms::setCntctId2(int pCntctId)
{
  _cntctId2 = pCntctId;
}

void Alarms::setCntctId3(int pCntctId)
{
  _cntctId3 = pCntctId;
}

void Alarms::setDate(QDate pDate)
{
  _dueDate = pDate;

  XSqlQuery q;
  
  q.prepare("SELECT saveAlarm(alarm_id, alarm_number,"
            "                 :alarm_date, LOCALTIME, alarm_time_offset, alarm_time_qualifier,"
            "                 alarm_event, alarm_event_recipient,"
            "                 alarm_email, alarm_email_recipient,"
            "                 alarm_sysmsg, alarm_sysmsg_recipient,"
            "                 alarm_source, alarm_source_id, 'CHANGEALL') AS result "
            "FROM alarm "
            "WHERE ( (alarm_source=:alarm_source)"
            "  AND   (alarm_source_id=:alarm_source_id) ); ");

  q.bindValue(":alarm_date", _dueDate);
  q.bindValue(":alarm_source", Alarms::_alarmMap[_source].ident);
  q.bindValue(":alarm_source_id", _sourceid);
  q.exec();
  
  refresh();
}

void Alarms::setReadOnly(bool pReadOnly)
{
  if (_readOnly == pReadOnly)
    return;

  if(pReadOnly){
    disconnect(_alarms, SIGNAL(valid(bool)), _edit, SLOT(setEnabled(bool)));
    disconnect(_alarms, SIGNAL(valid(bool)), _delete, SLOT(setEnabled(bool)));
    disconnect(_alarms, SIGNAL(doubleClicked(QModelIndex)), _edit, SLOT(animateClick()));
    connect(_alarms, SIGNAL(doubleClicked(QModelIndex)), _view, SLOT(animateClick()));
  }
  else
  {
    connect(_alarms, SIGNAL(valid(bool)), _edit, SLOT(setEnabled(bool)));
    connect(_alarms, SIGNAL(valid(bool)), _delete, SLOT(setEnabled(bool)));
    connect(_alarms, SIGNAL(doubleClicked(QModelIndex)), _edit, SLOT(animateClick()));
    disconnect(_alarms, SIGNAL(doubleClicked(QModelIndex)), _view, SLOT(animateClick()));
  }
  _new->setEnabled(!pReadOnly);
  _edit->setEnabled(!pReadOnly);
  _delete->setEnabled(!pReadOnly);
  
}

void Alarms::sNew()
{
  ParameterList params;
  params.append("mode", "new");
  params.append("source", _source);
  params.append("source_id", _sourceid);
  params.append("due_date", _dueDate);
  params.append("usrId1", _usrId1);
  params.append("usrId2", _usrId2);
  params.append("usrId3", _usrId3);
  params.append("cntctId1", _cntctId1);
  params.append("cntctId2", _cntctId2);
  params.append("cntctId3", _cntctId3);

  alarmMaint newdlg(this, "", true);
  newdlg.set(params);

  if (newdlg.exec() != QDialog::Rejected)
    refresh();
}

void Alarms::sEdit()
{
  ParameterList params;
  params.append("mode", "edit");
  params.append("alarm_id", _alarms->id());

  alarmMaint newdlg(this, "", true);
  newdlg.set(params);

  if (newdlg.exec() != QDialog::Rejected)
    refresh();
}

void Alarms::sView()
{
  ParameterList params;
  params.append("mode", "view");
  params.append("alarm_id", _alarms->id());

  alarmMaint newdlg(this, "", true);
  newdlg.set(params);
  newdlg.exec();
}

void Alarms::sDelete()
{
  XSqlQuery q;
  q.prepare( "DELETE FROM alarm "
             "WHERE (alarm_id=:alarm_id);" );
  q.bindValue(":alarm_id", _alarms->id());
  q.exec();

  refresh();
}

void Alarms::refresh()
{
  if(-1 == _sourceid)
  {
    _alarms->clear();
    return;
  }

  XSqlQuery query;
  
  //Populate alarms
  QString sql( "SELECT alarm_id,"
               "       CASE WHEN (alarm_time_offset > 0) THEN"
               "                 CAST(alarm_time_offset AS TEXT) || ' ' || "
               "                 CASE WHEN (alarm_time_qualifier = 'MB') THEN :minutesbefore"
               "                      WHEN (alarm_time_qualifier = 'HB') THEN :hoursbefore"
               "                      WHEN (alarm_time_qualifier = 'DB') THEN :daysbefore"
               "                      WHEN (alarm_time_qualifier = 'MA') THEN :minutesafter"
               "                      WHEN (alarm_time_qualifier = 'HA') THEN :hoursafter"
               "                      WHEN (alarm_time_qualifier = 'DA') THEN :daysafter"
               "                 END"
               "            ELSE '' "
               "       END AS f_offset, alarm_time,"
               "       CASE WHEN (alarm_event) THEN alarm_event_recipient END AS alarm_event_recipient,"
               "       CASE WHEN (alarm_email) THEN alarm_email_recipient END AS alarm_email_recipient,"
               "       CASE WHEN (alarm_sysmsg) THEN alarm_sysmsg_recipient END AS alarm_sysmsg_recipient "
               "FROM alarm "
               "WHERE ( (alarm_source=:source) "
               "  AND   (alarm_source_id=:sourceid) ) "
               "ORDER BY alarm_time; ");
  query.prepare(sql);
  query.bindValue(":email", tr("Email"));
  query.bindValue(":event", tr("Event"));
  query.bindValue(":sysmsg", tr("System Message"));
  query.bindValue(":other", tr("Other"));
  query.bindValue(":minutesbefore", tr("minutes before"));
  query.bindValue(":hoursbefore", tr("hours before"));
  query.bindValue(":daysbefore", tr("days before"));
  query.bindValue(":minutesafter", tr("mintues after"));
  query.bindValue(":hoursafter", tr("hours after"));
  query.bindValue(":daysafter", tr("days after"));
  query.bindValue(":source", _alarmMap[_source].ident);
  query.bindValue(":sourceid", _sourceid);
  query.exec();
  _alarms->populate(query);
}

// script exposure /////////////////////////////////////////////////////////////

void setupAlarms(QScriptEngine *engine)
{
  QScriptValue widget = engine->newObject();

  widget.setProperty("Uninitialized",	 QScriptValue(engine, Alarms::Uninitialized),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Address",	         QScriptValue(engine, Alarms::Address),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("BBOMHead",	 QScriptValue(engine, Alarms::BBOMHead),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("BBOMItem",	 QScriptValue(engine, Alarms::BBOMItem),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("BOMHead",	         QScriptValue(engine, Alarms::BOMHead),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("BOMItem",	         QScriptValue(engine, Alarms::BOMItem),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("BOOHead",	         QScriptValue(engine, Alarms::BOOHead),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("BOOItem",	         QScriptValue(engine, Alarms::BOOItem),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("CRMAccount",	 QScriptValue(engine, Alarms::CRMAccount),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Contact",	         QScriptValue(engine, Alarms::Contact),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Customer",	 QScriptValue(engine, Alarms::Customer),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Employee",	 QScriptValue(engine, Alarms::Employee),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Incident",	 QScriptValue(engine, Alarms::Incident),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Item",	         QScriptValue(engine, Alarms::Item),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("ItemSite",	 QScriptValue(engine, Alarms::ItemSite),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("ItemSource",	 QScriptValue(engine, Alarms::ItemSource),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Location",	 QScriptValue(engine, Alarms::Location),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("LotSerial",	 QScriptValue(engine, Alarms::LotSerial),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Opportunity",	 QScriptValue(engine, Alarms::Opportunity),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Project",	         QScriptValue(engine, Alarms::Project),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("PurchaseOrder",	 QScriptValue(engine, Alarms::PurchaseOrder),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("PurchaseOrderItem",QScriptValue(engine, Alarms::PurchaseOrderItem),QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("ReturnAuth",	 QScriptValue(engine, Alarms::ReturnAuth),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("ReturnAuthItem",	 QScriptValue(engine, Alarms::ReturnAuthItem),   QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Quote",	         QScriptValue(engine, Alarms::Quote),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("QuoteItem",	 QScriptValue(engine, Alarms::QuoteItem),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("SalesOrder",	 QScriptValue(engine, Alarms::SalesOrder),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("SalesOrderItem",	 QScriptValue(engine, Alarms::SalesOrderItem),   QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("TodoItem",	 QScriptValue(engine, Alarms::TodoItem),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("TransferOrder",	 QScriptValue(engine, Alarms::TransferOrder),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("TransferOrderItem",QScriptValue(engine, Alarms::TransferOrderItem),QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Vendor",	         QScriptValue(engine, Alarms::Vendor),	         QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("Warehouse",	 QScriptValue(engine, Alarms::Warehouse),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);
  widget.setProperty("WorkOrder",	 QScriptValue(engine, Alarms::WorkOrder),	 QScriptValue::ReadOnly | QScriptValue::Undeletable);

  engine->globalObject().setProperty("Alarms", widget, QScriptValue::ReadOnly | QScriptValue::Undeletable);
}

