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
    if( m_var != nullptr ){
        connect( m_var, &VarPlus::richNameChanged, this, &QLabelRichName::setText );
        connect( m_var, &VarPlus::destroyed, this, &QLabelRichName::resetVar );
        connect( m_var, &VarPlus::toolTipChanged, this, &QLabelRichName::setToolTip );
        setToolTip( m_var->toolTip() );
        setVisible( m_var->enabled() );
        connect( m_var,&VarPlus::enabledChanged, this, &QLabelRichName::setVisible );
        setText( m_var->richName() );
    }
}

void QLabelRichName::setToolTip( const QString & newTT ) {
    QLabel::setToolTip( newTT );
}

void QLabelRichName::disconnectVar() {
    if( m_var != nullptr ){
        disconnect( m_var, &VarPlus::richNameChanged, this, &QLabelRichName::setText );
        disconnect( m_var, &VarPlus::destroyed, this, &QLabelRichName::resetVar );
        disconnect( m_var, &VarPlus::toolTipChanged, this, &QLabelRichName::setToolTip );
        setToolTip( "" );
        setVisible( true );
        disconnect( m_var,&VarPlus::enabledChanged, this, &QLabelRichName::setVisible );
        clear();

        m_var = nullptr;
    }
}

void QLabelRichName::resetVar() {
    if( m_var != 0 ){
        clear();
        m_var = 0;
    }
}
