#include "../../Interface/inc/field.h"
#include <QLineEdit>
#include <QLabel>
#include <QDebug>


/**
 * @brief Constrói uma nova instância da classe Field.
 * 
 * Insere os valores mínimos e máximos do campo de texto.
 * 
 * @param parent instância da classe experimento criada na classe MainWindow.
 */
Field::Field(Experiment *parent)
{

    info_variables = parent;
    this->FieldMinimumSize[0] = QSize(100, 30);
    this->FieldMaximumSize[0] = QSize(1000, 100);
}
/**
 * @brief Customização dos campos de texto com a label superior de um layout escolhido.
 * 
 * 
 * @param gridLayout Layout escolhido.
 */
void Field::customizeField(QLayout *gridLayout)
{
    QLayout *layout = gridLayout->layout();
    if (layout) {
        for (int i = 0; i < layout->count(); ++i){
            QLayout * fieldLayout = layout->itemAt(i)->layout();

            QLineEdit * lineEdit = qobject_cast<QLineEdit*>(fieldLayout->itemAt(1)->widget());
            QLabel * label = qobject_cast<QLabel*>(fieldLayout->itemAt(0)->widget());

            if(lineEdit && label){
                //qDebug() << lineEdit->objectName();
                customizeOneField(label, lineEdit);
            }


       }
    }

}

/**
 * @brief Realiza a customização de um campo de texto e de uma label.
 * 
 * Conecta os campos de texto ao SLOT editingFinished().
 * 
 * @param label Label escolhida.
 * @param lineEdit Campo de texto escolhido.
 */
void Field::customizeOneField(QLabel *label, QLineEdit *lineEdit){
    //lineEdit->setMaximumSize(this->lineEditMaximumSize[size]);
    lineEdit->setAlignment(Qt::AlignTop);
    lineEdit->setMinimumSize(this->FieldMinimumSize[0]);
    lineEdit->setMaximumSize(this->FieldMaximumSize[0]);
    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lineEdit->setFont(QFont ( "Ubuntu", 13, QFont::Normal));

    QFont font( "Ubuntu", 15, QFont::Bold);
    label->setFont(font);
    label->setStyleSheet("color: #10576D;");
    label->setAlignment(Qt::AlignBottom);


    connect(lineEdit, SIGNAL(editingFinished()),this, SLOT(setVariables()));
}

/**
 * @brief Limpa os campos de texto de uma layout escolhido.
 * 
 * @param gridLayout Layout escolhido.
 */
void Field::clearFields(QLayout *gridLayout)
{
    QLayout *layout = gridLayout->layout();
    if (layout) {
        for (int i = 0; i < layout->count(); ++i){
            QLayout * fieldLayout = layout->itemAt(i)->layout();

            QLineEdit * lineEdit = qobject_cast<QLineEdit*>(fieldLayout->itemAt(1)->widget());
            lineEdit->setText("");


       }
    }
}

/**
 * @brief Define os valores das variáveis do experimento com base nos campos de texto.
 * 
 * Esse slot é disparado toda vez que o sinal 
 * editingFinished() é emitido pelos campos de texto.
 */
void Field::setVariables()
{
    QLineEdit* field = qobject_cast<QLineEdit*>(sender());
    QString fieldNames[] = {
        "experimentName_lineEdit", "operator_lineEdit", "testType_lineEdit",
        "specimenType_lineEdit", "uscs_lineEdit", "ashto_lineEdit",
        "samplePreparation_lineEdit", "sampleId_lineEdit", "boringNumber_lineEdit",
        "sampleLocation_lineEdit", "sampleDescription_lineEdit", "height_lineEdit",
        "wetWeight_lineEdit", "humidity_lineEdit", "spgr_lineEdit",
        "plastic_lineEdit", "liquid_lineEdit", "diameter_lineEdit", "pressure_lineEdit",
    };


    //p->(32);
//    typedef void (Experiment:*functionType)(void);
//    functionType func[3] = {&Experiment::setName};

    for( int i=0;i<sizeof(fieldNames)/sizeof(QString);i++){
        if(field->objectName() == fieldNames[i]){
            switch(i){
                case 0:
                    info_variables->setName(field->text());
                break;
                case 1:
                    info_variables->setOperator_name(field->text());
                break;
                case 2:
                    info_variables->setTest_type(field->text());
                break;
                case 3:
                    info_variables->setSpecimen_type(field->text());
                break;
                case 4:
                    info_variables->setUscs_class(field->text());
                break;
                case 5:
                    info_variables->setAshto_class(field->text());
                break;
                case 6:
                    info_variables->setSample_preparations(field->text());
                break;
                case 7:
                    info_variables->setSample_id(field->text().toInt());
                break;
                case 8:
                    info_variables->setBoring_number(field->text().toInt());
                break;
                case 9:
                    info_variables->setSample_location(field->text());
                break;
                case 10:
                    info_variables->setSample_description(field->text());
                break;
                case 11:
                    info_variables->setInitial_height(field->text().toFloat());
                break;
                case 12:
                    info_variables->setInitial_wet_weight(field->text().toFloat());
                break;
                case 13:
                    info_variables->setInitial_moisture(field->text().toFloat());
                break;
                case 14:
                    info_variables->setSpgr_solids(field->text().toFloat());
                break;
                case 15:
                    info_variables->setPlastic_limit(field->text().toFloat());
                break;
                case 16:
                    info_variables->setLiquid_limit(field->text().toFloat());
                break;
                case 17:
                    info_variables->setDiameter(field->text().toFloat());
                break;
                case 18:
                    info_variables->setPressure(field->text().toFloat());
                default:
                break;

            }
        }
    }


}
