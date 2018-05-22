#include "vtkviewer.h"

#include "vtkviewermulti.h"

#include <QVTKWidget.h>

#include <QMenu>
#include <QContextMenuEvent>
#include <QToolButton>
#include <QGridLayout>
#include <QHBoxLayout>

class VTKViewerPrivate {
public:
    VTKViewerPrivate() {
        splitMeHorizontalAction = NULL;
        splitMeVerticalAction = NULL;
    }
    /** Azione per chiedere al genitore di tipo GLWidgetMulti di dividere il widget
        in orizzontale */
    QAction * splitMeHorizontalAction;
    /** Azione per chiedere al genitore di tipo GLWidgetMulti di dividere il widget
        in verticale */
    QAction * splitMeVerticalAction;
};

VTKViewer::VTKViewer ( QWidget * parent ):
    QWidget( parent ),
    m_d( new VTKViewerPrivate() ){
    QGridLayout * gridLayout = new QGridLayout(this);

    QHBoxLayout * horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

    QToolButton * toolButton = new QToolButton(this);
    toolButton->setObjectName(QString::fromUtf8("toolButton"));
    horizontalLayout->addWidget(toolButton);

    QToolButton * toolButton_2 = new QToolButton(this);
    toolButton_2->setObjectName(QString::fromUtf8("toolButton_2"));
    horizontalLayout->addWidget(toolButton_2);

    QToolButton * toolButton_3 = new QToolButton(this);
    toolButton_3->setObjectName(QString::fromUtf8("toolButton_3"));
    horizontalLayout->addWidget(toolButton_3);

    QSpacerItem * horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

    QVTKWidget * widget = new QVTKWidget( this );
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
    widget->setSizePolicy(sizePolicy);

    widget->setContextMenuPolicy( Qt::NoContextMenu );

    gridLayout->addWidget(widget, 1, 0, 1, 1);

    initActions();
}

void VTKViewer::initActions(){
    VTKViewerMulti * multiParent = dynamic_cast<VTKViewerMulti *>(parent());
    if( multiParent ){
        m_d->splitMeHorizontalAction  = new QAction( trUtf8("Orizzontale"), this );
        connect( m_d->splitMeHorizontalAction, &QAction::triggered, this, &VTKViewer::emitSplit );

        m_d->splitMeVerticalAction  = new QAction( trUtf8("Verticale"), this );
        connect( m_d->splitMeVerticalAction, &QAction::triggered, this, &VTKViewer::emitSplit );
    }
}

void VTKViewer::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);


    // menu.addSection( trUtf8("Colori") );
    // menu.addAction(m_d->setBgColorAction);
    // menu.addAction(m_d->setDivColorAction);

    // menu.addSection( trUtf8("Navigazione") );
    // menu.addAction(m_d->showSysRefIcon);
    // menu.addAction(m_d->showButtonsAction);

    if( m_d->splitMeHorizontalAction != NULL && m_d->splitMeVerticalAction != NULL ){
        menu.addSection( trUtf8("Dividi vista") );
        menu.addAction( m_d->splitMeHorizontalAction );
        menu.addAction( m_d->splitMeVerticalAction );
    }

    menu.exec(event->globalPos());
}

void VTKViewer::emitSplit() {
    if( m_d->splitMeHorizontalAction == sender() ){
        emit splitMeHorizontal( this );
    } else if( m_d->splitMeVerticalAction == sender() ){
        emit splitMeVertical( this );
    }
}
