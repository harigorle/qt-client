/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2012 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef ITEMSOURCELIST_H
#define ITEMSOURCELIST_H

#include "xdialog.h"
#include "guiclient.h"
#include <parameter.h>

#include "ui_itemSourceList.h"

class itemSourceList : public XDialog, public Ui::itemSourceList
{
    Q_OBJECT

public:
    itemSourceList(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0);
    ~itemSourceList();

public slots:
    virtual enum SetResponse set(const ParameterList & pParams);
    virtual void sSelect();
    virtual void sFillList();

protected slots:
    virtual void languageChange();

};

#endif // ITEMSOURCELIST_H
