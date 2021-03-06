/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2017 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __SCRIPTABLEWIDGET_H__
#define __SCRIPTABLEWIDGET_H__

#include <QString>

#include "parameter.h"

class QScriptEngine;
class QScriptEngineDebugger;

class ScriptableWidget
{
  public:
    ScriptableWidget(QObject *object);
    virtual ~ScriptableWidget();

    virtual QScriptEngine *engine();
    virtual void           loadScript(const QStringList &list);
    virtual void           loadScript(const QString     &oName);
    virtual void           loadScriptEngine();

  protected:
    QScriptEngineDebugger *_debugger;
    QScriptEngine         *_engine;
    QObject               *_object;
    bool                   _scriptLoaded;
};

#endif
