#include "unitmeasuregui.h"

#include "unitmeasure.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVector>

UnitMeasureGUI::UnitMeasureGUI( UnitMeasure * extModel, QWidget *parent) :
        QDialog(parent),
        m_model(extModel),
        m_scaleLEVector( new QVector<QLineEdit *> ),
        m_strLEVector( new QVector<QLineEdit *> ),
        m_richStrLEVector( new QVector<QLineEdit *> ){
    buildGUI();
}

void UnitMeasureGUI::buildGUI(){
    setWindowTitle( trUtf8( "Unità di misura utente"));
    QGridLayout * layout = new QGridLayout( this );
    layout->addWidget( new QLabel( trUtf8( "Descrizione"), this ), 0, 0);
    layout->addWidget( new QLabel( trUtf8( "Rapporto"), this ), 0, 1);
    layout->addWidget( new QLabel( trUtf8( "Simbolo"), this ), 0, 2);
    layout->addWidget( new QLabel( trUtf8( "Simbolo HTML"), this ), 0, 3);
    int i = 0;
    for( i=0; i < m_model->size(); i++){
        layout->addWidget( new QLabel( toHTML( m_model->description(i)), this ), (i+1), 0 );
        QLineEdit * scaleLE = new QLineEdit( QString::number( m_model->scale(i) ), this );
        m_scaleLEVector->append( scaleLE );
        layout->addWidget( scaleLE, (i+1), 1 );
        QLineEdit * strLE = new QLineEdit( m_model->string(i), this  );
        m_strLEVector->append( strLE );
        layout->addWidget( strLE, (i+1), 2 );
        QLineEdit * richStrLE = new QLineEdit( m_model->richString(i), this );
        m_richStrLEVector->append(richStrLE );
        layout->addWidget( richStrLE, (i+1), 3 );
    }

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    layout->addWidget( buttonBox, (i+1), 3 );
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout( layout );
}

void UnitMeasureGUI::accept(){
    for(int i=0; i < m_model->size(); i++){
        bool ok = false;
        double val = m_scaleLEVector->at(i)->text().toDouble( &ok );
        if( ok ){
            m_model->setScale( i, val );
        }
        m_model->setString( i, m_strLEVector->at(i)->text());
        m_model->setRichString(i, m_richStrLEVector->at(i)->text());
    }
    QDialog::accept();
}

QString UnitMeasureGUI::toHTML( const QString & str ){
    QString ret;
    ret.append( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> \n" );
    ret.append( "<html>\n" );
    ret.append( "<head><meta name=\"qrichtext\" content=\"1\" /></head>" );
    ret.append( "<body>\n" );
    ret.append( str );
    ret.append( "</body>\n" );
    ret.append("</html>" );
    return ret;
}
