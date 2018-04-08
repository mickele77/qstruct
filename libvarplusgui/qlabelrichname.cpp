#include "qlabelrichname.h"

#include "varplus.h"

QLabelRichName::QLabelRichName(QWidget *parent, VarPlus * v ) :
    QLabel(parent),
    QWidgetPlus(v) {
    connectVar();
}

QLabelRichName::~QLabelRichName(){
}

void QLabelRichName::setVar(VarPlus *v) {
    if( m_var != v ){
        disconnectVar();
        m_var = v;
        connectVar();
    }
}

void QLabelRichName::connectVar() {
    if( m_var != NULL ){
        connect( m_var, SIGNAL(richNameChanged(QString)), this, SLOT(setText(QString)) );
        connect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        connect( m_var, SIGNAL(toolTipChanged(QString)), this, SLOT(setToolTip(QString)) );
        setToolTip( m_var->toolTip() );
        setVisible( m_var->enabled() );
        connect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        setText( m_var->richName() );
    }
}

void QLabelRichName::setToolTip( const QString & newTT ) {
    QLabel::setToolTip( newTT );
}

void QLabelRichName::disconnectVar() {
    if( m_var != 0 ){
        disconnect( m_var, SIGNAL(richNameChanged(QString)), this, SLOT(setText(QString)) );
        disconnect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        disconnect( m_var, SIGNAL(toolTipChanged(QString)), this, SLOT(setToolTip(QString)) );
        setToolTip( "" );
        setVisible( true );
        disconnect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        clear();

        m_var = 0;
    }
}

void QLabelRichName::resetVar() {
    if( m_var != 0 ){
        clear();
        m_var = 0;
    }
}
