#include "../../Interface/inc/mainwindow.h"
#include "ui_mainwindow.h"
#include "inc/field.h"
#include<QDebug>
#include <QLineEdit>
#include <QSignalMapper>
#include "inc/experiment.h"
#include <QSqlTableModel>
#include <QTextEdit>
#include "inc/charts.h"

/**
 * @brief Constrói uma nova janela principal.
 * 
 * É o construtor da janela principal da interface.
 * 
 * @param parent 
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainStack->setCurrentIndex(1);
    //ui->lineEdit->setInputMethodHints(inputMethodHints() | Qt::InputMethodHint::ImhDigitsOnly);
    this->setupButtons = new Button();

    this->info_variables = new Experiment();
    this->setupFields = new Field(this->info_variables);
    this->CreateDataseTables();
    this->tables = new Table(this->info_variables,ui->densification_tableWidget, ui->shear_tableWidget);
    this->data_export = new exportData();
    this->charts_variables = new Charts();

    InitialConfiguration_OutsideExperimentHeaderButtons();
    InitialConfiguration_InsideExperimentButtons();
    InitialConfiguration_PhasesButtons();
    InitialConfiguration_PhasesFields();
    InitialConfiguration_Tables();

    connect(ui->sampleDescription_lineEdit, SIGNAL(editingFinished()),this->setupFields, SLOT(setVariables()));
    send_data = new sendCommands();
    receive_data = new ThreadController(NULL,this->tables);

    for(int i=0;i<20;i++){
        if(!send_data->connectToMachine()){
            QThread::msleep(500);
        } else{
          break;
        }
    }

    if(send_data->errorOccurred || this->receive_data->receiveDataThread->errorOccurred){
        qDebug() << "Não foi possível se conectar à máquina.";
        delete send_data;
        delete receive_data;
        delete ui;
        exit(-1);
    }

    connect(this->receive_data->receiveDataThread,SIGNAL(data_arrived()),this->my_db, SLOT(update_database_table()));
    


    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(changeInitialPositionValue()));

    connect(ui->cancel_toolButton, SIGNAL(clicked()), this, SLOT(cancelExperiment()));

    fillTextEditForTests();

}


/**
 * @brief Destrói a janela principal.
 * 
 * Destrói a instância ui;
 * 
 */
MainWindow::~MainWindow()
{
    delete send_data;
    delete receive_data;
    delete ui;
}

void MainWindow::CreateDataseTables(){
    const QString path ="./database.db";
    qDebug() << "Antes de criar a tabela";
    my_db = new DBManager (path, this->info_variables);
    if(my_db->isOpen())
    {
        if(!my_db->tableExists(experiment_table)){my_db->createTable(experiment_table);}
        if(!my_db->tableExists(densification_table)){my_db->createTable(densification_table);}
        if(!my_db->tableExists(shear_table)){my_db->createTable(shear_table);}
        if(!my_db->tableExists(final_variables_table)){my_db->createTable(final_variables_table);}
        if(!my_db->tableExists(sample_table)){my_db->createTable(sample_table);}

    }
}

void MainWindow::InitialConfiguration_OutsideExperimentHeaderButtons()
{
    // Insert buttons styles on header outside experiment
    this->setupButtons->initialButtonStyling_Layout(ui->outside_experiment_header_layout, headerButton_lightBackgroundColor, outsideExperiment_buttonSize);
    // Mark experiment button as first page
    this->setupButtons->changeButton_style(ui->experimentButton,experimentButton_lightIcon,headerButton_lightBackgroundColor,0);
    ui->outside_experiment_stack->setCurrentIndex(2);
    connectButtonsToSlots_Layout(ui->outside_experiment_header_layout, SIGNAL(clicked()), SLOT(changeOutsideExperimentPage()));
}

void MainWindow::InitialConfiguration_InsideExperimentButtons()
{
    this->setupButtons->initialButtonStyling_Layout(ui->inside_experiment_header_layout, headerButton_lightBackgroundColor, outsideExperiment_buttonSize);
    this->setupButtons->initialButtonStyling_Layout(ui->densificationHeader_layout, phasesButton_lightBackgroundColor,phases_buttonSize);
    this->setupButtons->initialButtonStyling_Layout(ui->shearHeader_layout, phasesButton_lightBackgroundColor,phases_buttonSize);

    this->setupButtons->initialButtonStyling(ui->cancel_toolButton, cancelButton_BackgroundColor, cancelButton_size);
    this->setupButtons->initialButtonStyling(ui->initShear_FinishExperiment_toolButton, initShearButton_BackgroundColor, initShearButton_size );

    this->setupButtons->initialButtonStyling(ui->initShear_toolButton, initShearButton_BackgroundColor, phases_buttonSize );
    this->setupButtons->initialButtonStyling(ui->goBack_toolButton, phasesButton_lightBackgroundColor, phases_buttonSize );
    this->setupButtons->initialButtonStyling(ui->exportTable_toolButton, phasesButton_lightBackgroundColor, exportButton_size );

    this->setupButtons->initialButtonStyling_Layout(ui->adjustVelocity_Layout, velocityPositionBackgroundColor, velocityPositionButton_size);
    this->setupButtons->initialButtonStyling_Layout(ui->adjustDistance_Layout, velocityPositionBackgroundColor, velocityPositionButton_size);

    connectButtonToSlots_WithArguments(ui->inside_experiment_header_layout,ui->insideExperiment_stack,0);

    connectButtonToSlots_WithArguments(ui->densificationHeader_layout,ui->densification_stack,1);

    connectButtonToSlots_WithArguments(ui->shearHeader_layout,ui->shear_stack,2);

    connect(ui->initShear_FinishExperiment_toolButton, SIGNAL(clicked()), this, SLOT(adjustVelocity_Distance()));

    connect(ui->densificationResult_toolButton, SIGNAL(clicked()), this, SLOT(updateResultsTables()));
    connect(ui->shearResult_toolButton, SIGNAL(clicked()), this, SLOT(updateResultsTables()));

    connect(ui->initShear_toolButton, SIGNAL(clicked()), this, SLOT(initShearPhase()));

    connectButtonsToSlots_Layout(ui->adjustVelocity_Layout, SIGNAL(clicked()), SLOT(changeVelocity()));
    connectButtonsToSlots_Layout(ui->adjustDistance_Layout, SIGNAL(clicked()), SLOT(changeDistance()));


    connect(ui->insideExperiment_stack, SIGNAL(currentChanged(int)),this, SLOT(enableShearInitButton(int)));

    connect(ui->densification_stack, SIGNAL(currentChanged(int)),this, SLOT(changeExportOption_Densification(int)));
    connect(ui->shear_stack, SIGNAL(currentChanged(int)),this, SLOT(changeExportOption_Shear(int)));

    connect(ui->insideExperiment_stack, SIGNAL(currentChanged(int)),this, SLOT(enableExportButton(int)));

    this->charts_variables->initialConfiguration(ui->densificationGraphs_layout);
    connect(this->info_variables, SIGNAL(updateDensificationChart(int,float)), this->charts_variables, SLOT(updateCharts(int, float)));

    connect(ui->exportTable_toolButton, &QToolButton::clicked,  this->data_export, [this]{
            this->data_export->exportCSV(chosenTable,my_db->getExperiment_id(),this->export_option);
        }
    );



}

void MainWindow::InitialConfiguration_PhasesButtons()
{
    // Insert style of phases buttons
    this->setupButtons->initialButtonStyling_Layout(ui->phases_layout, phasesButton_lightBackgroundColor,phases_buttonSize);
    this->setupButtons->changeButton_style(ui->phase1_button,no_icon,phasesButton_lightBackgroundColor,1);
    ui->phases_stack->setCurrentIndex(0);
    connectButtonsToSlots_Layout(ui->phases_layout, SIGNAL(clicked()), SLOT(changePhase()));

    this->setupButtons->initialButtonStyling_Widget(ui->phases_stack, continueButton_BackgroundColor, phases_continueButtonSize);

    this->setupButtons->initExperiment_ButtonStyle(ui->initExperiment_toolButton,false);
    ui->initExperiment_toolButton->setDisabled(true);
    this->setupButtons->pressureButton_style(ui->releasePressure_toolButton);

    //this->setupButtons->setButton_style_icon(ui->continuePhase1_button, continueButton_BackgroundColor, continueButton_Icon);
    //this->setupButtons->setButtonShadow(ui->continuePhase1_button);
    connectButtonsToSlots_Widget(ui->phase1_page, SIGNAL(clicked()),SLOT(nextPhase()));
    connectButtonsToSlots_Widget(ui->phase2_page, SIGNAL(clicked()),SLOT(nextPhase()));
    connectButtonsToSlots_Widget(ui->phase3_page, SIGNAL(clicked()),SLOT(nextPhase()));
    connectButtonsToSlots_Widget(ui->calculations_page, SIGNAL(clicked()),SLOT(nextPhase()));
    connect(ui->continuePosition_Button, SIGNAL(clicked()),this, SLOT(nextPhase()));


    //connectButtonsToSlots_Layout(ui->positionLayout, SIGNAL(clicked()), SLOT(changeInitialPositionValue()));
    connectButtonsToSlots_Layout(ui->positionLayout, SIGNAL(pressed()), SLOT(onPositionButton_pressed()));
    connectButtonsToSlots_Layout(ui->positionLayout, SIGNAL(released()), SLOT(onPositionButton_released()));

}

void MainWindow::InitialConfiguration_PhasesFields()
{
    this->setupFields->customizeField(ui->phase1_gridLayout);
    this->setupFields->customizeField(ui->phase2_gridLayout);
    this->setupFields->customizeField(ui->phase3_gridLayout);

    //this->setupFields->customizeOneField(ui->initialPosition_label, ui->initialPosition_lineEdit);
    //ui->initialPosition_lineEdit->setAlignment(Qt::AlignCenter);
    //ui->initialPosition_lineEdit->setFont(QFont ( "Ubuntu", 30, QFont::Normal));
}

void MainWindow::InitialConfiguration_Tables()
{
    this->tables->customizeTable(ui->phases_tableWidget);
    //this->tables->initialConfig_TablePhases(ui->phases_tableWidget);
    this->tables->initialConfig_StaticTable(ui->phases_tableWidget,phases_table);
    this->tables->customizeTable(ui->info_tableWidget);
    this->tables->initialConfig_StaticTable(ui->info_tableWidget,info_table);

    this->tables->customizeTable(ui->shear_tableWidget);
    this->tables->customizeTable(ui->densification_tableWidget);
    this->tables->initialConfig_DynamicTable(ui->densification_tableWidget,design_densification_table);
    this->tables->initialConfig_DynamicTable(ui->shear_tableWidget,design_shear_table);


    this->tables->customizeTable(ui->densificationResult_tableWidget);
    this->tables->initialConfig_StaticTable(ui->densificationResult_tableWidget,densification_result_table);

    this->tables->customizeTable(ui->shearResult_tableWidget);
    this->tables->initialConfig_StaticTable(ui->shearResult_tableWidget,shear_result_table);
    // this->tables->initialConfig_ShearTable(ui->shear_tableWidget);

    //this->tables->exportCSV();


}

void MainWindow::connectButtonToSlots_WithArguments(QHBoxLayout * boxlayout, QStackedWidget * stack_widget, int option)
{

    QLayout *layout = boxlayout->layout();
    if (layout) {
        for (int i = 0; i < layout->count(); ++i){
           QToolButton * button = qobject_cast<QToolButton*>(layout->itemAt(i)->widget());
           if(button){
               connect(button, &QToolButton::clicked,  this->setupButtons, [this, stack_widget, option]{
                   if(option==0){
                       this->setupButtons->changeHeaderPage_InsideExperiment(stack_widget);
                   } else{
                       this->setupButtons->changePage_InsideExperiment(stack_widget, option==1);
                   }
                   });
           }
       }
    }





}

void MainWindow::connectButtonsToSlots_Layout(QHBoxLayout *boxlayout, const char *signal, const char *slot)
{

    QLayout *layout = boxlayout->layout();
    if (layout) {
        for (int i = 0; i < layout->count(); ++i){
           QToolButton * button = qobject_cast<QToolButton*>(layout->itemAt(i)->widget());

           if(button){
                connect(button, signal,this,slot);
           }
       }
    }
}

void MainWindow::connectButtonsToSlots_Widget(QObject *selectedWidget, const char *signal, const char *slot)
{

   QList<QToolButton*> selectedButtons = selectedWidget->findChildren<QToolButton*>();
   for(QList<QToolButton *>::iterator buttons = selectedButtons.begin();buttons != selectedButtons.end(); buttons++){
       connect(*buttons, signal,this,slot);
   }
   return;
}

void MainWindow::changePage(QToolButton *buttonSender, QString buttons_name[6], uint8_t array_size, QStackedWidget * page_stack, uint8_t icon[], uint8_t style)
{
    QString object_name = buttonSender->objectName();
    int next_page = 0;
    for(int i=0;i<array_size;i++){
        if(object_name == buttons_name[i]){
            next_page = i;
        }
    }
    uint8_t choosen_icon = icon?icon[next_page]:no_icon;
    uint8_t pos = icon? 0:1;
    page_stack->setCurrentIndex(next_page);
    this->setupButtons->changeButton_style(buttonSender, choosen_icon, style, pos);

    if(next_page == 3){
        //this->tables->updateData_TablePhases(ui->phases_tableWidget);
        this->tables->updateData_StaticTable(ui->phases_tableWidget,phases_table);
    }
}

void MainWindow::nextPhase()
{
    int current_page = ui->phases_stack->currentIndex();
    int next_page = current_page + 1;

    QToolButton * button = qobject_cast<QToolButton*>(ui->phases_layout->itemAt(next_page)->widget());
    ui->phases_stack->setCurrentIndex(next_page);
    this->setupButtons->changeButton_style(button,no_icon,phasesButton_lightBackgroundColor, 1);

    if(next_page == 3){
        //this->tables->updateData_TablePhases(ui->phases_tableWidget);
        this->tables->updateData_StaticTable(ui->phases_tableWidget,phases_table);
    }
}

void MainWindow::changePhase()
{
    QToolButton* buttonSender = qobject_cast<QToolButton*>(sender()); // retrieve the button you have clicked
    QString buttons_name[6] = {"phase1_button","phase2_button","phase3_button","calculations_button","position_button","pressure_button"};
    uint8_t array_size = sizeof(buttons_name)/sizeof(buttons_name[0]);
    changePage(buttonSender, buttons_name, array_size, ui->phases_stack, NULL, phasesButton_lightBackgroundColor);
}

void MainWindow::changeOutsideExperimentPage()
{
    QToolButton* buttonSender = qobject_cast<QToolButton*>(sender()); // retrieve the button you have clicked
    QString buttons_name[] = {"historyButton","configurationButton","experimentButton"};
    uint8_t icon[3] = {historyButton_lightIcon,configurationButton_lightIcon,experimentButton_lightIcon};
    uint8_t array_size = sizeof(buttons_name)/sizeof(buttons_name[0]);
    changePage(buttonSender, buttons_name, array_size, ui->outside_experiment_stack, icon,headerButton_lightBackgroundColor);
}

void MainWindow::changeInitialPositionValue()
{
    qDebug() << "Estou no displacement";
    ui->initialPositionValue_label->setText(QString::number(receive_data->receiveDataThread->machine_message.displacement[1]));
    qDebug() << receive_data->receiveDataThread->machine_message.displacement[1];
}

void MainWindow::onPositionButton_pressed()
{
    int sampling_period = 100;
    timer->start(sampling_period);
    QToolButton* buttonSender = qobject_cast<QToolButton*>(sender());


    float velocity = 10.0;
    if(buttonSender->objectName() == "moveLeft_toolButton"){
        velocity *= -1;
    }
    send_data->setCommand(0);
    send_data->setEnabled(1);
    send_data->setSamplingPeriod(sampling_period);
    send_data->sendMessage();

    send_data->setCommand(1);
    send_data->setVelocity(velocity);
    send_data->sendMessage();


}

void MainWindow::onPositionButton_released()
{

    send_data->setCommand(1);
    send_data->setVelocity(0);
    send_data->sendMessage();

    send_data->setCommand(0);
    send_data->setEnabled(0);
    send_data->setSamplingPeriod(0);
    send_data->sendMessage();


    timer->stop();
}



void MainWindow::on_initExperiment_toolButton_clicked()
{
    this->info_variables->setInitial_time(true);
    this->info_variables->setExperimentStarted(true);
    this->info_variables->setInitial_position(ui->initialPositionValue_label->text().toFloat());
    uint32_t diff = this->info_variables->densification_variables.getSample_number();
    this->info_variables->setSample_period(1000);
    this->info_variables->densification_variables.setDiff_sampleNumber_initExperiment(diff==0?diff:diff+1);
    
    
    ui->mainStack->setCurrentIndex(0);
    ui->insideExperiment_stack->setCurrentIndex(0);
    ui->densification_stack->setCurrentIndex(0);
    ui->shear_stack->setCurrentIndex(0);
    this->setupButtons->changeButton_style(ui->densification_button, densificationButton_lightIcon, headerButton_lightBackgroundColor,0);
    this->setupButtons->changeButton_style(ui->densificationGraphs_toolButton, no_icon, phasesButton_lightBackgroundColor,1);
    this->setupButtons->changeButton_style(ui->shearGraphs_toolButton, no_icon, phasesButton_lightBackgroundColor,2);
    this->tables->updateData_StaticTable(ui->info_tableWidget,info_table);
    if(my_db->isOpen()){
        my_db->insertIntoTable(experiment_table);
        my_db->insertIntoTable(sample_table);
    }


    //QThread::sleep(5);

    send_data->setCommand(3);
    send_data->setPressure(this->info_variables->getPressure());
    send_data->sendMessage();

    send_data->setCommand(0);
    send_data->setEnabled(1);
    send_data->setSamplingPeriod(this->info_variables->getSample_period());
    send_data->sendMessage();
}


void MainWindow::on_releasePressure_toolButton_clicked()
{




        bool isPositionButtonsEnable = !(ui->releasePressure_toolButton->isChecked());
        int style = 0;
        if(isPositionButtonsEnable){
            style = pressureButton_GreenBackgroundColor;
            ui->releasePressure_toolButton->setText("Liberar a baixa pressão");
        } else {
            style = pressureButton_RedBackgroundColor;
            ui->releasePressure_toolButton->setText("Parar a baixa pressão");
        }

        if(!this->experiment_canceled){
            this->send_data->setCommand(2);
            this->send_data->sendMessage();
        }
        


        this->setupButtons->initExperiment_ButtonStyle(ui->initExperiment_toolButton,!isPositionButtonsEnable);
        ui->initExperiment_toolButton->setDisabled(isPositionButtonsEnable);

        this->setupButtons->setButton_style_icon(ui->releasePressure_toolButton, style , no_icon );


        QLayout *layout = ui->positionLayout->layout();
        if (layout) {
            for (int i = 0; i < layout->count(); ++i){
               QToolButton * button = qobject_cast<QToolButton*>(layout->itemAt(i)->widget());

               if(button){
                    button->setEnabled(isPositionButtonsEnable);
                    int move_button_style =  isPositionButtonsEnable? continueButton_BackgroundColor:moveButton_DisabledBackgroundColor;
                    this->setupButtons->setButton_style_icon(button, move_button_style, no_icon);
               }
           }
        }

}

void MainWindow::updateResultsTables()
{
    if(this->info_variables->getPhase() == densification_phase)
    {
       this->tables->updateData_StaticTable(ui->densificationResult_tableWidget,densification_result_table);
    } else if (this->info_variables->getPhase() == shear_phase)
    {
       this->tables->updateData_StaticTable(ui->shearResult_tableWidget,shear_result_table);
    }

}

void MainWindow::adjustVelocity_Distance()
{
    qDebug() << "Consegui clicar aqui";
    if(this->info_variables->getPhase() == densification_phase)
    {
        //ui->initShear_FinishExperiment_toolButton->
        ui->initShear_FinishExperiment_toolButton->setChecked(true);
        this->previousIndex =  ui->insideExperiment_stack->currentIndex();
        ui->insideExperiment_stack->setCurrentIndex(3);
        //ui->initShear_FinishExperiment_toolButton->setVisible(false);
    } else if (this->info_variables->getPhase() == shear_phase)
    {
        if(my_db->isOpen()){
            my_db->insertIntoTable(final_variables_table);
        }

        qDebug() << "Experimento Finalizado";
        this->cancelExperiment();
    }


}

void MainWindow::on_goBack_toolButton_clicked()
{
    ui->insideExperiment_stack->setCurrentIndex(previousIndex);
}

void MainWindow::initShearPhase()
{
   
    // manda comandos
    send_data->setCommand(4);
    send_data->setVelocity(this->info_variables->shear_variables.getVelocity());
    send_data->setDistance(this->info_variables->shear_variables.getDistance());
    send_data->sendMessage();

   
    // altera estilo do botão 
    this->setupButtons->changeInitShear_toFinishButton(ui->initShear_FinishExperiment_toolButton);
    //this->ui->initShear_FinishExperiment_toolButton->setIcon(button_icons[icon]);

    uint32_t sample_number = this->info_variables->densification_variables.getSample_number();
    this->info_variables->shear_variables.setSample_number_diff(sample_number+1);

    if(my_db->isOpen()){
        my_db->updateTable(sample_table);
    }


    this->info_variables->changePhase();
    this->ui->phase_label->setText("Fase: Cisalhamento");
    ui->insideExperiment_stack->setCurrentIndex(1);
    this->setupButtons->changeButton_style(ui->shear_button, shearButton_lightIcon, headerButton_lightBackgroundColor,0);
    this->setupButtons->changeButton_style(ui->shearTable_toolButton, no_icon , phasesButton_lightBackgroundColor,2);
    ui->shear_stack->setCurrentIndex(1);

    this->info_variables->shear_variables.setInitial_time();


    
}
void MainWindow::fillTextEditForTests()
{

    static int j=0;
    QStringList data = {
            "Experimento 3",
            "Iva",
            "Teso tipo 2",
            "Espme do tipo 2",
            "Cla uscs 35",
            "Clae ashto 3",
            "Nenma preparação",
            "123", "12",
            "Locação da amostra",
            "Descrição da amostra",
            "500","2","3","4","5",
            "6", "7", "8",
    };

    QStringList data2 = {
            "Experimento 10",
            "Luisa",
            "Teste do tipo 0000",
            "Espécime do tipo 0000",
            "Cla uscs 0000",
            "Clae ashto 0000",
            "Nenma preparação",
            "456", "21",
            "Locação da amostra",
            "Descrição da amostra",
            "5","8","2","3","4",
            "12", "13", "14",
    };

    QStringList array_data;
    if(j&1)
    {
        array_data = data;
    } else {
        array_data = data2;
    }
 


    

    int i=0;
    ui->experimentName_lineEdit->setText(array_data[i++]);
    ui->operator_lineEdit->setText(array_data[i++]);
    ui->testType_lineEdit->setText(array_data[i++]);
    ui->specimenType_lineEdit->setText(array_data[i++]);
    ui->uscs_lineEdit->setText(array_data[i++]);
    ui->ashto_lineEdit->setText(array_data[i++]);
    ui->samplePreparation_lineEdit->setText(array_data[i++]);
    ui->sampleId_lineEdit->setText(array_data[i++]);
    ui->boringNumber_lineEdit->setText(array_data[i++]);
    ui->sampleLocation_lineEdit->setText(array_data[i++]);
    ui->sampleDescription_lineEdit->setText(array_data[i++]);
    ui->height_lineEdit->setText(array_data[i++]);
    ui->wetWeight_lineEdit->setText(array_data[i++]);
    ui->humidity_lineEdit->setText(array_data[i++]);
    ui->spgr_lineEdit->setText(array_data[i++]);
    ui->plastic_lineEdit->setText(array_data[i++]);
    ui->liquid_lineEdit->setText(array_data[i++]);
    ui->diameter_lineEdit->setText(array_data[i++]);
    //qDebug() << "Valor da pressure" << array_data[i];
    ui->pressure_lineEdit->setText(array_data[i++]);
    i=0;

    info_variables->setName(array_data[i++]);
    info_variables->setOperator_name(array_data[i++]);
    info_variables->setTest_type(array_data[i++]);
    info_variables->setSpecimen_type(array_data[i++]);
    info_variables->setUscs_class(array_data[i++]);
    info_variables->setAshto_class(array_data[i++]);
    info_variables->setSample_preparations(array_data[i++]);
    info_variables->setSample_id(array_data[i++].toInt());
    info_variables->setBoring_number(array_data[i++].toInt());
    info_variables->setSample_location(array_data[i++]);
    info_variables->setSample_description(array_data[i++]);
    info_variables->setInitial_height(array_data[i++].toFloat());
    info_variables->setInitial_wet_weight(array_data[i++].toFloat());
    info_variables->setInitial_moisture(array_data[i++].toFloat());
    info_variables->setSpgr_solids(array_data[i++].toFloat());
    info_variables->setPlastic_limit(array_data[i++].toFloat());
    info_variables->setLiquid_limit(array_data[i++].toFloat());
    info_variables->setDiameter(array_data[i++].toFloat());
    info_variables->setPressure(array_data[i++].toFloat());



    //phase1_gridLayout
    // phase2_gridLayout
    // layout_SampleDescription
    // phase3_gridLayout

    j++;
   
    return;
}

void MainWindow:: cancelExperiment()
{
    
    this->experiment_canceled = true;
    // sockets
    send_data->setCommand(5);
    send_data->sendMessage();

    //qDebug() <<"Area = " <<  this->info_variables->getArea();
    //delete this->info_variables;
    this->info_variables = new Experiment();
    this->setupFields->info_variables = this->info_variables;
    this->tables->table_variables = this->info_variables;
    this->my_db->experiment_data = this->info_variables;
    //qDebug() <<"Area = " <<   this->info_variables->getArea();
    
    // Densification temp;
    // this->info_variables->densification_variables = temp;

    this->info_variables->densification_variables.setDensificationVariables(0,0,0);

    ui->releasePressure_toolButton->setChecked(false);
    this->on_releasePressure_toolButton_clicked();
    ui->sampleDescription_lineEdit->setText("");
    ui->initialPositionValue_label->setText("0");
    

    ui->mainStack->setCurrentIndex(1);
    ui->outside_experiment_stack->setCurrentIndex(2);
    ui->phases_stack->setCurrentIndex(0);

    this->setupButtons->changeButton_style(ui->experimentButton,experimentButton_lightIcon,headerButton_lightBackgroundColor,0);
    this->setupButtons->changeButton_style(ui->phase1_button,no_icon,phasesButton_lightBackgroundColor,1);
    //ui->lineEdit->setInputMethodHints(inputMethodHints() | Qt::InputMethodHint::ImhDigitsOnly);



    this->setupFields->clearFields(ui->phase1_gridLayout);
    this->setupFields->clearFields(ui->phase2_gridLayout);
    this->setupFields->clearFields(ui->phase3_gridLayout);

    this->tables->clearDynamicTables(ui->densification_tableWidget);
    this->tables->clearDynamicTables(ui->shear_tableWidget);
    this->tables->clearStaticTables(ui->densificationResult_tableWidget);

    

    this->tables->clearStaticTables(ui->shearResult_tableWidget);

    this->ui->velocityValue_label->setText("0");
    this->ui->distanceValue_label->setText("0");

    this->ui->initShear_FinishExperiment_toolButton->setChecked(false);
    this->setupButtons->changeInitShear_toFinishButton(ui->initShear_FinishExperiment_toolButton);

    // QList<QTableWidget*> selectedTables = ui->mainStack->findChildren<QTableWidget*>();
    // for(QList<QTableWidget *>::iterator table = selectedTables.begin();table != selectedTables.end(); table++){
    //     qDebug() << "Tabela = " << (*table)->objectName();
    // }

    fillTextEditForTests();
   
    this->experiment_canceled = false;



}


void MainWindow::changeVelocity()
{
    QToolButton* buttonSender = qobject_cast<QToolButton*>(sender());
    QString buttons_names[] = {
        "minus100Velocity_toolButton",
        "minus10Velocity_toolButton",
        "minus1Velocity_toolButton",
        "plus1Velocity_toolButton",
        "plus10Velocity_toolButton",
        "plus100Velocity_toolButton"
    };
    int values[] = {-100,-10,-1,1,10,100};
    int add=0;

    for(unsigned long int i=0;i< sizeof(buttons_names)/sizeof(QString);i++)
    {
        if(buttonSender->objectName() == buttons_names[i])
        {
            add = values[i];
        }
    }

    int32_t current_velocity = this->info_variables->shear_variables.getVelocity();
    current_velocity += add;
    if(current_velocity <= ((1<<16) -1)){
        this->info_variables->shear_variables.setVelocity(current_velocity);
        this->ui->velocityValue_label->setText(QString::number(this->info_variables->shear_variables.getVelocity()));
    }


}

void MainWindow::changeDistance()
{
    QToolButton* buttonSender = qobject_cast<QToolButton*>(sender());
    QString buttons_names[] = {
        "minus100Distance_toolButton",
        "minus10Distance_toolButton",
        "minus1Distance_toolButton",
        "plus1Distance_toolButton",
        "plus10Distance_toolButton",
        "plus100Distance_toolButton"
    };
    int values[] = {-100,-10,-1,1,10,100};
    int add=0;

    for(unsigned long int i=0;i< sizeof(buttons_names)/sizeof(QString);i++)
    {
        if(buttonSender->objectName() == buttons_names[i])
        {
            add = values[i];
        }
    }

    int32_t current_distance = this->info_variables->shear_variables.getDistance();
    current_distance += add;
    if(current_distance <= ((1<<16) -1)){
        this->info_variables->shear_variables.setDistance(current_distance);
        this->ui->distanceValue_label->setText(QString::number(this->info_variables->shear_variables.getDistance()));
    }


}

void MainWindow::enableShearInitButton(int index)
{
    bool isButtonVisible = ui->initShear_FinishExperiment_toolButton->isVisible();
    if(isButtonVisible && index == 3){
        ui->initShear_FinishExperiment_toolButton->setVisible(false);
        ui->exportTable_toolButton->setVisible(false);
    } else if(index != 3 && !isButtonVisible){
        ui->initShear_FinishExperiment_toolButton->setVisible(true);
    } 
}

void MainWindow::enableExportButton(int index)
{
    if(index==3){
        ui->exportTable_toolButton->setVisible(false);
    } else if(index ==2) {
        this->export_option = 0;
        ui->exportTable_toolButton->setVisible(true);
        chosenTable = ui->info_tableWidget;
    } else if(index == 1){
        int inside_index = ui->shear_stack->currentIndex();

        if(this->info_variables->getPhase() == densification_phase || inside_index ==0)
        {
            ui->exportTable_toolButton->setVisible(false);
        } else {
            chosenTable = (inside_index==1)? ui->shear_tableWidget :ui->shearResult_tableWidget;
            this->export_option = inside_index + 2;
            ui->exportTable_toolButton->setVisible(true);
        }

    } else if(index == 0){
        int inside_index = ui->densification_stack->currentIndex();

        if(inside_index >= 1){
            chosenTable = (inside_index==1)? ui->densification_tableWidget:ui->densificationResult_tableWidget ;
            this->export_option = inside_index;
            ui->exportTable_toolButton->setVisible(true);
        } else{
            ui->exportTable_toolButton->setVisible(false);
        }
    }
}

void MainWindow::changeExportOption_Densification(int index)
{

    if(index >= 1){
        chosenTable = (index==1)? ui->densification_tableWidget:ui->densificationResult_tableWidget ;
        this->export_option = index;
        ui->exportTable_toolButton->setVisible(true);
    } else{
        ui->exportTable_toolButton->setVisible(false);
    }
}

void MainWindow::changeExportOption_Shear(int index)
{
    //qDebug() << "Dentro da página";
    if(this->info_variables->getPhase() == densification_phase)
    {
        ui->exportTable_toolButton->setVisible(false);
    } else if(index >= 1){
        chosenTable = (index==1)? ui->shear_tableWidget :ui->shearResult_tableWidget;
        this->export_option = index + 2;
        ui->exportTable_toolButton->setVisible(true);
    } else{
        ui->exportTable_toolButton->setVisible(false);
    }
}

